//
//  ToneMapAndResample.h
//  libraries/render-utils/src
//
//  Created by Anna Brewer on 7/3/19.
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_ToneMapAndResample_h
#define hifi_ToneMapAndResample_h

#include <DependencyManager.h>
#include <NumericalConstants.h>

#include <gpu/Resource.h>
#include <gpu/Pipeline.h>
#include <render/Forward.h>
#include <render/DrawTask.h>

enum class ToneCurve {
    // Different tone curve available
    None,
    Gamma22,
    Reinhard,
    Filmic,
    Piecewise
};

struct CurveParamsUser
{
    float m_toeStrength; // as a ratio
    float m_toeLength; // as a ratio
    float m_shoulderStrength; // as a ratio
    float m_shoulderLength; // in F stops
    float m_shoulderAngle; // as a ratio

    float m_gamma;
};

struct CurveSegment {
    float m_offsetX;
    float m_offsetY;
    float m_scaleX; // always 1 or -1
    float m_scaleY;
    float m_lnA;
    float m_B;
};

struct FullCurve
{
    float m_W;
    float m_invW;

    float m_x0;
    float m_x1;
    float m_y0;
    float m_y1;
};

class ToneMappingConfig : public render::Job::Config {
    Q_OBJECT
    Q_PROPERTY(float exposure MEMBER exposure WRITE setExposure);
    Q_PROPERTY(int curve MEMBER curve WRITE setCurve);
    Q_PROPERTY(float toeStrength MEMBER toeStrength WRITE setToeStrength);
    Q_PROPERTY(float toeLength MEMBER toeLength WRITE setToeLength);
    Q_PROPERTY(float shoulderStrength MEMBER shoulderStrength WRITE setShoulderStrength);
    Q_PROPERTY(float shoulderLength MEMBER shoulderLength WRITE setShoulderLength);
    Q_PROPERTY(float shoulderAngle MEMBER shoulderAngle WRITE setShoulderAngle);
    Q_PROPERTY(float gamma MEMBER gamma WRITE setGamma);

public:
    ToneMappingConfig() : render::Job::Config(true) {}

    void setExposure(float newExposure) { exposure = newExposure; emit dirty(); }
    void setCurve(int newCurve) { curve = std::max((int)ToneCurve::None, std::min((int)ToneCurve::Piecewise, newCurve)); emit dirty(); }
    void setToeStrength(float newToeStrength) { toeStrength = newToeStrength; emit dirty(); }
    void setToeLength(float newToeLength) { toeLength = newToeLength; emit dirty(); }
    void setShoulderStrength(float newShoulderStrength) { shoulderStrength = newShoulderStrength; emit dirty(); }
    void setShoulderLength(float newShoulderLength) { shoulderLength = newShoulderLength; emit dirty(); }
    void setShoulderAngle(float newShoulderAngle) { shoulderAngle = newShoulderAngle; emit dirty(); }
    void setGamma(float newGamma) { gamma = newGamma; emit dirty(); }

    float exposure{ 0.0f };
    float toeStrength{ 0.5f };
    float toeLength{ 0.5f };

    float shoulderStrength{ 0.5f };
    float shoulderLength{ 0.5f };
    float shoulderAngle{ 1.0f };
    float gamma{ 2.2f };

    int curve{ (int)ToneCurve::Gamma22 };

signals:
    void dirty();
};

class ToneMapAndResample {
public:
    ToneMapAndResample();
    virtual ~ToneMapAndResample() {}

    void render(RenderArgs* args, const gpu::TexturePointer& lightingBuffer, gpu::FramebufferPointer& destinationBuffer);

    void setExposure(float exposure);
    float getExposure() const { return pow(_parametersBuffer.get<Parameters>()._twoPowExposure, 0.5); }

    void setToneCurve(ToneCurve curve);
    ToneCurve getToneCurve() const { return (ToneCurve)_parametersBuffer.get<Parameters>()._toneCurve; }

    void setToeStrength(float strength);
    float getToeStrength() const { return userParams.m_toeStrength; }

    void setToeLength(float strength);
    float getToeLength() const { return userParams.m_toeLength; }

    void setShoulderStrength(float strength);
    float getShoulderStrength() const { return userParams.m_shoulderStrength; }

    void setShoulderLength(float strength);
    float getShoulderLength() const { return userParams.m_shoulderLength; }

    void setShoulderAngle(float strength);
    float getShoulderAngle() const { return userParams.m_shoulderAngle; }

    void setGamma(float strength);
    float getGamma() const { return userParams.m_gamma; }

    // Inputs: lightingFramebuffer, destinationFramebuffer
    using Input = render::VaryingSet2<gpu::FramebufferPointer, gpu::FramebufferPointer>;
    using Output = gpu::FramebufferPointer;
    using Config = ToneMappingConfig;
    using JobModel = render::Job::ModelIO<ToneMapAndResample, Input, Output, Config>;

    void configure(const Config& config);
    void run(const render::RenderContextPointer& renderContext, const Input& input, Output& output);

    CurveSegment m_segments[3];

protected:
    static gpu::PipelinePointer _pipeline;
    static gpu::PipelinePointer _mirrorPipeline;
    static gpu::PipelinePointer _piecewisePipeline;
    static gpu::PipelinePointer _piecewiseMirrorPipeline;

    gpu::FramebufferPointer _destinationFrameBuffer;

    float _factor{ 2.0f };

    gpu::FramebufferPointer getResampledFrameBuffer(const gpu::FramebufferPointer& sourceFramebuffer);

    bool _dirty;

    void setCurveParams(FullCurve curve);

    CurveParamsUser userParams = { 0.5, 0.5, 2.0, 0.5, 1.0, 2.2 };

private:
    gpu::PipelinePointer _blitLightBuffer;

    // Class describing the uniform buffer with all the parameters common to the tone mapping shaders
    class Parameters {
    public:
        float _shoulderOffsetX;
        float _shoulderOffsetY;
        float _shoulderLnA;
        float _shoulderB;

        float _toeLnA;
        float _toeB;

        float _linearLnA;
        float _linearB;
        float _linearOffsetX;

        float _twoPowExposure = 1.0f;
 
        float _fullCurveW;
        float _fullCurveInvW;
        float _fullCurveX0;
        float _fullCurveY0;
        float _fullCurveX1;
        float _fullCurveY1;

        int _toneCurve = (int)ToneCurve::Gamma22;

        Parameters() {}
    };

    typedef gpu::BufferView UniformBufferView;
    gpu::BufferView _parametersBuffer;

    void init();
};

#endif // hifi_ToneMapAndResample_h
