//
//  Created by Sam Gondelman on 5/16/19
//  Copyright 2013-2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "RenderScriptingInterface.h"

#include "LightingModel.h"
#include "AntialiasingEffect.h"

const QString DEFERRED = "deferred";
const QString FORWARD = "forward";

RenderScriptingInterface* RenderScriptingInterface::getInstance() {
    static RenderScriptingInterface sharedInstance;
    return &sharedInstance;
}

RenderScriptingInterface::RenderScriptingInterface() {
    setRenderMethod((render::Args::RenderMethod)_renderMethodSetting.get() == render::Args::RenderMethod::DEFERRED ? DEFERRED : FORWARD);
    setShadowsEnabled(_shadowsEnabledSetting.get());
    setAmbientOcclusionEnabled(_ambientOcclusionEnabledSetting.get());
    setAntialiasingEnabled(_antialiasingEnabledSetting.get());
}

QString RenderScriptingInterface::getRenderMethod() {
    return (render::Args::RenderMethod)_renderMethodSetting.get() == render::Args::RenderMethod::DEFERRED ? DEFERRED : FORWARD;
}

void RenderScriptingInterface::setRenderMethod(const QString& renderMethod) {
    render::Args::RenderMethod newMethod = renderMethod == FORWARD ? render::Args::RenderMethod::FORWARD : render::Args::RenderMethod::DEFERRED;
    if (_renderMethodSetting.get() == newMethod) {
        return;
    }

    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setRenderMethod", Q_ARG(const QString&, renderMethod));
        return;
    }

    auto config = dynamic_cast<task::SwitchConfig*>(qApp->getRenderEngine()->getConfiguration()->getConfig("RenderMainView.DeferredForwardSwitch"));
    if (config) {
        _renderMethodSetting.set(newMethod);
        config->setBranch(newMethod);
        emit config->dirtyEnabled();
    }
}

bool RenderScriptingInterface::getShadowsEnabled() {
    return _shadowsEnabledSetting.get();
}

void RenderScriptingInterface::setShadowsEnabled(bool enabled) {
    if (_shadowsEnabledSetting.get() == enabled) {
        return;
    }

    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setShadowsEnabled", Q_ARG(bool, enabled));
        return;
    }

    auto lightingModelConfig = qApp->getRenderEngine()->getConfiguration()->getConfig<MakeLightingModel>("RenderMainView.LightingModel");
    if (lightingModelConfig) {
        Menu::getInstance()->setIsOptionChecked(MenuOption::Shadows, enabled);
        _shadowsEnabledSetting.set(enabled);
        lightingModelConfig->setShadow(enabled);
    }
}

bool RenderScriptingInterface::getAmbientOcclusionEnabled() {
    return _ambientOcclusionEnabledSetting.get();
}

void RenderScriptingInterface::setAmbientOcclusionEnabled(bool enabled) {
    if (_ambientOcclusionEnabledSetting.get() == enabled) {
        return;
    }

    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setAmbientOcclusionEnabled", Q_ARG(bool, enabled));
        return;
    }

    auto lightingModelConfig = qApp->getRenderEngine()->getConfiguration()->getConfig<MakeLightingModel>("RenderMainView.LightingModel");
    if (lightingModelConfig) {
        Menu::getInstance()->setIsOptionChecked(MenuOption::AmbientOcclusion, enabled);
        _ambientOcclusionEnabledSetting.set(enabled);
        lightingModelConfig->setAmbientOcclusion(enabled);
    }
}

bool RenderScriptingInterface::getAntialiasingEnabled() {
    return _antialiasingEnabledSetting.get();
}

void RenderScriptingInterface::setAntialiasingEnabled(bool enabled) {
    if (_antialiasingEnabledSetting.get() == enabled) {
        return;
    }

    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "setAntialiasingEnabled", Q_ARG(bool, enabled));
        return;
    }

    auto mainViewJitterCamConfig = qApp->getRenderEngine()->getConfiguration()->getConfig<JitterSample>("RenderMainView.JitterCam");
    auto mainViewAntialiasingConfig = qApp->getRenderEngine()->getConfiguration()->getConfig<Antialiasing>("RenderMainView.Antialiasing");
    if (mainViewJitterCamConfig && mainViewAntialiasingConfig) {
        Menu::getInstance()->setIsOptionChecked(MenuOption::AntiAliasing, enabled);
        _antialiasingEnabledSetting.set(enabled);
        if (enabled) {
            mainViewJitterCamConfig->play();
            mainViewAntialiasingConfig->setDebugFXAA(false);
        } else {
            mainViewJitterCamConfig->none();
            mainViewAntialiasingConfig->setDebugFXAA(true);
        }
    }
}