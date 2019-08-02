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
    float getExposure() const { return _parametersBuffer.get<Parameters>()._exposure; }

    void setToneCurve(ToneCurve curve);
    ToneCurve getToneCurve() const { return (ToneCurve)_parametersBuffer.get<Parameters>()._toneCurve; }

    void ToneMapAndResample::setToeStrength(float strength);
    float getToeStrength() const { return _parametersBuffer.get<Parameters>()._toeStrength; }

    void ToneMapAndResample::setToeLength(float strength);
    float getToeLength() const { return _parametersBuffer.get<Parameters>()._toeLength; }

    void ToneMapAndResample::setShoulderStrength(float strength);
    float getShoulderStrength() const { return _parametersBuffer.get<Parameters>()._shoulderStrength; }

    void ToneMapAndResample::setShoulderLength(float strength);
    float getShoulderLength() const { return _parametersBuffer.get<Parameters>()._shoulderLength; }

    void ToneMapAndResample::setShoulderAngle(float strength);
    float getShoulderAngle() const { return _parametersBuffer.get<Parameters>()._shoulderAngle; }

    void ToneMapAndResample::setGamma(float strength);
    float getGamma() const { return _parametersBuffer.get<Parameters>()._gamma; }

    // Inputs: lightingFramebuffer, destinationFramebuffer
    using Input = render::VaryingSet2<gpu::FramebufferPointer, gpu::FramebufferPointer>;
    using Output = gpu::FramebufferPointer;
    using Config = ToneMappingConfig;
    using JobModel = render::Job::ModelIO<ToneMapAndResample, Input, Output, Config>;

    void configure(const Config& config);
    void run(const render::RenderContextPointer& renderContext, const Input& input, Output& output);

protected:
    static gpu::PipelinePointer _pipeline;
    static gpu::PipelinePointer _mirrorPipeline;
    static gpu::PipelinePointer _piecewisePipeline;
    static gpu::PipelinePointer _piecewiseMirrorPipeline;

    gpu::FramebufferPointer _destinationFrameBuffer;

    float _factor{ 2.0f };

    gpu::FramebufferPointer getResampledFrameBuffer(const gpu::FramebufferPointer& sourceFramebuffer);

private:
    gpu::PipelinePointer _blitLightBuffer;

    // Class describing the uniform buffer with all the parameters common to the tone mapping shaders
    class Parameters {
    public:
        float _exposure = 0.0f;
        float _twoPowExposure = 1.0f;
        float _toeStrength = 0.5f;
        float _toeLength = 0.5f;

        float _shoulderStrength = 2.0f;
        float _shoulderLength = 0.5f;
        float _shoulderAngle = 1.0f;
        float _gamma = 2.2f;

        int _toneCurve = (int)ToneCurve::Gamma22;

        Parameters() {}
    };

    typedef gpu::BufferView UniformBufferView;
    gpu::BufferView _parametersBuffer;

    void init();
};

#endif // hifi_ToneMapAndResample_h
