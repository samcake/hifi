//
//  LODManager.h
//  interface/src/LODManager.h
//
//  Created by Clement on 1/16/15.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_LODManager_h
#define hifi_LODManager_h

#include <DependencyManager.h>
#include <NumericalConstants.h>
#include <OctreeConstants.h>
#include <PIDController.h>
#include <SimpleMovingAverage.h>
#include <render/Args.h>

const float DEFAULT_DESKTOP_LOD_DOWN_FPS = 30.0f;
const float DEFAULT_HMD_LOD_DOWN_FPS = 34.0f;
const float DEFAULT_DESKTOP_MAX_RENDER_TIME = (float)MSECS_PER_SECOND / DEFAULT_DESKTOP_LOD_DOWN_FPS; // msec
const float DEFAULT_HMD_MAX_RENDER_TIME = (float)MSECS_PER_SECOND / DEFAULT_HMD_LOD_DOWN_FPS; // msec
const float MAX_LIKELY_DESKTOP_FPS = 61.0f; // this is essentially, V-synch + 1 fps
const float MAX_LIKELY_HMD_FPS = 91.0f; // this is essentially, V-synch + 1 fps
const float INCREASE_LOD_GAP_FPS = 10.0f; // fps

// The default value DEFAULT_OCTREE_SIZE_SCALE means you can be 400 meters away from a 1 meter object in order to see it (which is ~20:20 vision).
const float ADJUST_LOD_MAX_SIZE_SCALE = DEFAULT_OCTREE_SIZE_SCALE;
// This controls how low the auto-adjust LOD will go. We want a minimum vision of ~20:500 or 0.04 of default
const float ADJUST_LOD_MIN_SIZE_SCALE = DEFAULT_OCTREE_SIZE_SCALE * 0.04f;

class AABox;

class LODManager : public QObject, public Dependency {
    Q_OBJECT
    SINGLETON_DEPENDENCY

public:
    Q_INVOKABLE void setAutomaticLODAdjust(bool value) { _automaticLODAdjust = value; }
    Q_INVOKABLE bool getAutomaticLODAdjust() const { return _automaticLODAdjust; }

    Q_INVOKABLE void setDesktopLODDecreaseFPS(float value);
    Q_INVOKABLE float getDesktopLODDecreaseFPS() const;
    Q_INVOKABLE float getDesktopLODIncreaseFPS() const;

    Q_INVOKABLE void setHMDLODDecreaseFPS(float value);
    Q_INVOKABLE float getHMDLODDecreaseFPS() const;
    Q_INVOKABLE float getHMDLODIncreaseFPS() const;

    // User Tweakable LOD Items
    Q_INVOKABLE QString getLODFeedbackText();
    Q_INVOKABLE void setOctreeSizeScale(float sizeScale);
    Q_INVOKABLE float getOctreeSizeScale() const { return _octreeSizeScale; }

    Q_INVOKABLE void setBoundaryLevelAdjust(int boundaryLevelAdjust);
    Q_INVOKABLE int getBoundaryLevelAdjust() const { return _boundaryLevelAdjust; }

    Q_PROPERTY(float displayTargetFPS READ getDisplayTargetFPS)

    Q_INVOKABLE float getLODDecreaseFPS() const;
    Q_INVOKABLE float getLODIncreaseFPS() const;

    Q_PROPERTY(float presentTime READ getPresentTime)
    Q_PROPERTY(float renderTime READ getRenderTime)
    Q_PROPERTY(float batchTime READ getBatchTime)
    Q_PROPERTY(float gpuTime READ getGPUTime)

    Q_PROPERTY(float displayFPS READ getDisplayFPS)
    Q_PROPERTY(float engineFPS READ getEngineFPS)
    Q_PROPERTY(float lodLevel READ getLODLevel)
    Q_PROPERTY(float increaseSpeed MEMBER _increaseSpeed)
    Q_PROPERTY(float decreaseSpeed MEMBER _decreaseSpeed)

    Q_PROPERTY(float lodDecreaseFPS READ getLODDecreaseFPS)
    Q_PROPERTY(float lodIncreaseFPS READ getLODIncreaseFPS)

    Q_PROPERTY(float pidControlKp READ getPIDControlKp WRITE setPIDControlKp)
    Q_PROPERTY(float pidControlKi READ getPIDControlKi WRITE setPIDControlKi)
    Q_PROPERTY(float pidControlKd READ getPIDControlKd WRITE setPIDControlKd)

    Q_PROPERTY(float pidError READ getPIDError)
    Q_PROPERTY(float pidFeedbackP READ getPIDFeedbackP)
    Q_PROPERTY(float pidFeedbackI READ getPIDFeedbackI)
    Q_PROPERTY(float pidFeedbackD READ getPIDFeedbackD)
    Q_PROPERTY(float octreeSizeScale READ getOctreeSizeScale)

    void setPIDControlKp(float value);
    float getPIDControlKp() const;
    void setPIDControlKi(float value);
    float getPIDControlKi() const;
    void setPIDControlKd(float value);
    float getPIDControlKd() const;

    float getPIDError() const;
    float getPIDFeedbackP() const;
    float getPIDFeedbackI() const;
    float getPIDFeedbackD() const;

    float getDisplayTargetFPS() const { return _displayTargetFPS; }

    float getPresentTime() const { return _presentTime; }
    float getRenderTime() const { return _renderTime; }
    float getBatchTime() const { return _batchTime; }
    float getGPUTime() const { return _gpuTime; }

    static bool shouldRender(const RenderArgs* args, const AABox& bounds);
    void setRenderTimes(float presentTime, float renderTime, float batchTime, float gpuTime);
    void autoAdjustLOD(float realTimeDelta, float displayTargetFPS);

    void loadSettings();
    void saveSettings();
    void resetLODAdjust();

    float getLODLevel() const;

    float getDisplayFPS() const { return (float)MSECS_PER_SECOND / _avgDisplayTime; };
    float getEngineFPS() const { return (float)MSECS_PER_SECOND / _avgEngineTime; };


signals:
    void LODIncreased();
    void LODDecreased();

private:
    LODManager();

    PIDController _PIDController;

    bool _automaticLODAdjust = true;

    float _displayTargetFPS{ 60.0f };

    float _presentTime { 0.0f }; // msec
    float _renderTime { 0.0f }; // msec
    float _batchTime{ 0.0f }; // msec
    float _gpuTime { 0.0f }; // msec'

    float _avgDisplayTime{ 0.0f }; // msec
    float _avgEngineTime { 0.0f }; // msec

    float _desktopMaxRenderTime { DEFAULT_DESKTOP_MAX_RENDER_TIME };
    float _hmdMaxRenderTime { DEFAULT_HMD_MAX_RENDER_TIME };

    float _octreeSizeScale = DEFAULT_OCTREE_SIZE_SCALE;
    int _boundaryLevelAdjust = 0;

    float _increaseSpeed{ 0.5f };
    float _decreaseSpeed{ 1.0f };

    uint64_t _decreaseFPSExpiry { 0 };
    uint64_t _increaseFPSExpiry { 0 };
};

#endif // hifi_LODManager_h
