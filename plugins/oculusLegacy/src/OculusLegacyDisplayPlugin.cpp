//
//  Created by Bradley Austin Davis on 2014/04/13.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "OculusLegacyDisplayPlugin.h"

#include <memory>

#include <QtWidgets/QMainWindow>
#include <QtOpenGL/QGLWidget>
#include <GLMHelpers.h>
#include <gl/GlWindow.h>
#include <QEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QOpenGLContext>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <PerfStat.h>
#include <gl/OglplusHelpers.h>
#include <ViewFrustum.h>

#include "plugins/PluginContainer.h"
#include "OculusHelpers.h"

using namespace oglplus;

const QString OculusLegacyDisplayPlugin::NAME("Oculus Rift (0.5)");

const QString & OculusLegacyDisplayPlugin::getName() const {
    return NAME;
}

OculusLegacyDisplayPlugin::OculusLegacyDisplayPlugin() {
}

uvec2 OculusLegacyDisplayPlugin::getRecommendedRenderSize() const {
    return _desiredFramebufferSize;
}

void OculusLegacyDisplayPlugin::preRender() {
    ovrHmd_GetEyePoses(_hmd, _frameIndex, _eyeOffsets, _eyePoses, &_trackingState);
    ovrHmd_BeginFrame(_hmd, _frameIndex);
    WindowOpenGLDisplayPlugin::preRender();
}

glm::mat4 OculusLegacyDisplayPlugin::getProjection(Eye eye, const glm::mat4& baseProjection) const {
    return _eyeProjections[eye];
}

void OculusLegacyDisplayPlugin::resetSensors() {
    ovrHmd_RecenterPose(_hmd);
}

glm::mat4 OculusLegacyDisplayPlugin::getEyeToHeadTransform(Eye eye) const {
    return toGlm(_eyePoses[eye]);
}

// Should NOT be used for rendering as this will mess up timewarp.  Use the getModelview() method above for
// any use of head poses for rendering, ensuring you use the correct eye
glm::mat4 OculusLegacyDisplayPlugin::getHeadPose() const {
    return toGlm(_trackingState.HeadPose.ThePose);
}


bool OculusLegacyDisplayPlugin::isSupported() const {
    if (!ovr_Initialize(nullptr)) {
        return false;
    }
    bool result = false;
    if (ovrHmd_Detect() > 0) {
        result = true;
    }

    auto hmd = ovrHmd_Create(0);
    if (hmd) {
        QPoint targetPosition{ hmd->WindowsPos.x, hmd->WindowsPos.y };
        auto screens = qApp->screens();
        for(int i = 0; i < screens.size(); ++i) {
            auto screen = screens[i];
            QPoint position = screen->geometry().topLeft();
            if (position == targetPosition) {
                _hmdScreen = i;
                break;
            }
        }
    }
  
    ovr_Shutdown();
    return result;
}

void OculusLegacyDisplayPlugin::activate() {
    if (!(ovr_Initialize(nullptr))) {
        Q_ASSERT(false);
        qFatal("Failed to Initialize SDK");
    }
    _hswDismissed = false;
    _hmd = ovrHmd_Create(0);
    if (!_hmd) {
        qFatal("Failed to acquire HMD");
    }
    
    glm::uvec2 eyeSizes[2];
    ovr_for_each_eye([&](ovrEyeType eye) {
        _eyeFovs[eye] = _hmd->MaxEyeFov[eye];
        ovrEyeRenderDesc erd = _eyeRenderDescs[eye] = ovrHmd_GetRenderDesc(_hmd, eye, _eyeFovs[eye]);
        ovrMatrix4f ovrPerspectiveProjection =
            ovrMatrix4f_Projection(erd.Fov, DEFAULT_NEAR_CLIP, DEFAULT_FAR_CLIP, ovrProjection_RightHanded);
        _eyeProjections[eye] = toGlm(ovrPerspectiveProjection);

        ovrPerspectiveProjection =
            ovrMatrix4f_Projection(erd.Fov, 0.001f, 10.0f, ovrProjection_RightHanded);
        _compositeEyeProjections[eye] = toGlm(ovrPerspectiveProjection);

        _eyeOffsets[eye] = erd.HmdToEyeViewOffset;
        eyeSizes[eye] = toGlm(ovrHmd_GetFovTextureSize(_hmd, eye, erd.Fov, 1.0f));
    });
    ovrFovPort combined = _eyeFovs[Left];
    combined.LeftTan = std::max(_eyeFovs[Left].LeftTan, _eyeFovs[Right].LeftTan);
    combined.RightTan = std::max(_eyeFovs[Left].RightTan, _eyeFovs[Right].RightTan);
    ovrMatrix4f ovrPerspectiveProjection =
        ovrMatrix4f_Projection(combined, DEFAULT_NEAR_CLIP, DEFAULT_FAR_CLIP, ovrProjection_RightHanded);
    _eyeProjections[Mono] = toGlm(ovrPerspectiveProjection);

    _desiredFramebufferSize = uvec2(
        eyeSizes[0].x + eyeSizes[1].x,
        std::max(eyeSizes[0].y, eyeSizes[1].y));

    _frameIndex = 0;

    if (!ovrHmd_ConfigureTracking(_hmd,
        ovrTrackingCap_Orientation | ovrTrackingCap_Position | ovrTrackingCap_MagYawCorrection, 0)) {
        qFatal("Could not attach to sensor device");
    }

    WindowOpenGLDisplayPlugin::activate();

    int screen = getHmdScreen();
    if (screen != -1) {
        _container->setFullscreen(qApp->screens()[screen]);
    }
    
    _window->installEventFilter(this);
    _window->makeCurrent();
    ovrGLConfig config; memset(&config, 0, sizeof(ovrRenderAPIConfig));
    auto& header = config.Config.Header;
    header.API = ovrRenderAPI_OpenGL;
    header.BackBufferSize = _hmd->Resolution;
    header.Multisample = 1;
    int distortionCaps = 0
        | ovrDistortionCap_TimeWarp
        ;

    memset(_eyeTextures, 0, sizeof(ovrTexture) * 2);
    ovr_for_each_eye([&](ovrEyeType eye) {
        auto& header = _eyeTextures[eye].Header;
        header.API = ovrRenderAPI_OpenGL;
        header.TextureSize = { (int)_desiredFramebufferSize.x, (int)_desiredFramebufferSize.y };
        header.RenderViewport.Size = header.TextureSize;
        header.RenderViewport.Size.w /= 2;
        if (eye == ovrEye_Right) {
            header.RenderViewport.Pos.x = header.RenderViewport.Size.w;
        }
    });

    #ifndef QT_NO_DEBUG
    ovrBool result =
    #endif
        ovrHmd_ConfigureRendering(_hmd, &config.Config, distortionCaps, _eyeFovs, _eyeRenderDescs);
    Q_ASSERT(result);
}

void OculusLegacyDisplayPlugin::deactivate() {
    _window->removeEventFilter(this);

    WindowOpenGLDisplayPlugin::deactivate();
    
    QScreen* riftScreen = nullptr;
    if (_hmdScreen >= 0) {
        riftScreen = qApp->screens()[_hmdScreen];
    }
    _container->unsetFullscreen(riftScreen);
    
    ovrHmd_Destroy(_hmd);
    _hmd = nullptr;
    ovr_Shutdown();
}

// DLL based display plugins MUST initialize GLEW inside the DLL code.
void OculusLegacyDisplayPlugin::customizeContext() {
    glewExperimental = true;
    glewInit();
    glGetError();
    WindowOpenGLDisplayPlugin::customizeContext();
}

void OculusLegacyDisplayPlugin::preDisplay() {
    _window->makeCurrent();
}

void OculusLegacyDisplayPlugin::display(GLuint finalTexture, const glm::uvec2& sceneSize) {
    ++_frameIndex;
    ovr_for_each_eye([&](ovrEyeType eye) {
        reinterpret_cast<ovrGLTexture&>(_eyeTextures[eye]).OGL.TexId = finalTexture;
    });
    ovrHmd_EndFrame(_hmd, _eyePoses, _eyeTextures);
}

// Pass input events on to the application
bool OculusLegacyDisplayPlugin::eventFilter(QObject* receiver, QEvent* event) {
    if (!_hswDismissed && (event->type() == QEvent::KeyPress)) {
        static ovrHSWDisplayState hswState;
        ovrHmd_GetHSWDisplayState(_hmd, &hswState);
        if (hswState.Displayed) {
            ovrHmd_DismissHSWDisplay(_hmd);
        } else {
            _hswDismissed = true;
        }
    }    
    return WindowOpenGLDisplayPlugin::eventFilter(receiver, event);
}

// FIXME mirroring tot he main window is diffucult on OSX because it requires that we
// trigger a swap, which causes the client to wait for the v-sync of the main screen running
// at 60 Hz.  This would introduce judder.  Perhaps we can push mirroring to a separate
// thread
// FIXME If we move to the 'batch rendering on a different thread' we can possibly do this.  
// however, we need to make sure it doesn't block the event handling.
void OculusLegacyDisplayPlugin::finishFrame() {
    _window->doneCurrent();
};

int OculusLegacyDisplayPlugin::getHmdScreen() const {
    return _hmdScreen;
}
