//
//  ToneMapAndResampleTask.cpp
//  libraries/render-utils/src
//
//  Created by Anna Brewer on 7/3/19.
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "ToneMapAndResampleTask.h"

#include <gpu/Context.h>
#include <shaders/Shaders.h>

#include "render-utils/ShaderConstants.h"
#include "StencilMaskPass.h"
#include "FramebufferCache.h"

using namespace render;
using namespace shader::gpu::program;
using namespace shader::render_utils::program;

gpu::PipelinePointer ToneMapAndResample::_pipeline;
gpu::PipelinePointer ToneMapAndResample::_mirrorPipeline;
gpu::PipelinePointer ToneMapAndResample::_piecewisePipeline;
gpu::PipelinePointer ToneMapAndResample::_piecewiseMirrorPipeline;

ToneMapAndResample::ToneMapAndResample() {
    Parameters parameters;
    _parametersBuffer = gpu::BufferView(std::make_shared<gpu::Buffer>(sizeof(Parameters), (const gpu::Byte*) &parameters));
}

void ToneMapAndResample::init() {
    // shared_ptr to gpu::State
    gpu::StatePointer blitState = gpu::StatePointer(new gpu::State());

    blitState->setDepthTest(gpu::State::DepthTest(false, false));
    blitState->setColorWriteMask(true, true, true, true);

    _pipeline = gpu::PipelinePointer(gpu::Pipeline::create(gpu::Shader::createProgram(toneMapping), blitState));
    _mirrorPipeline = gpu::PipelinePointer(gpu::Pipeline::create(gpu::Shader::createProgram(toneMapping_mirrored), blitState));
    _piecewisePipeline = gpu::PipelinePointer(gpu::Pipeline::create(gpu::Shader::createProgram(toneMappingPiecewise), blitState));
    _piecewiseMirrorPipeline = gpu::PipelinePointer(gpu::Pipeline::create(gpu::Shader::createProgram(toneMappingPiecewise_mirrored), blitState));
}

void ToneMapAndResample::setExposure(float exposure) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._exposure != exposure) {
        _parametersBuffer.edit<Parameters>()._exposure = exposure;
        _parametersBuffer.edit<Parameters>()._twoPowExposure = pow(2.0, exposure);
    }
}

void ToneMapAndResample::setToneCurve(ToneCurve curve) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._toneCurve != (int)curve) {
        _parametersBuffer.edit<Parameters>()._toneCurve = (int)curve;
    }
}

void ToneMapAndResample::setToeStrength(float strength) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._toeStrength != strength) {
        _parametersBuffer.edit<Parameters>()._toeStrength = strength;
    }
}

void ToneMapAndResample::setToeLength(float length) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._toeLength != length) {
        _parametersBuffer.edit<Parameters>()._toeLength = length;
    }
}

void ToneMapAndResample::setShoulderStrength(float strength) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._shoulderStrength != strength) {
        _parametersBuffer.edit<Parameters>()._shoulderStrength = strength;
    }
}

void ToneMapAndResample::setShoulderLength(float length) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._shoulderLength != length) {
        _parametersBuffer.edit<Parameters>()._shoulderLength = length;
    }
}

void ToneMapAndResample::setShoulderAngle(float angle) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._shoulderAngle != angle) {
        _parametersBuffer.edit<Parameters>()._shoulderAngle = angle;
    }
}

void ToneMapAndResample::setGamma(float gamma) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._gamma != gamma) {
        _parametersBuffer.edit<Parameters>()._gamma = gamma;
    }
}

void ToneMapAndResample::configure(const Config& config) {
    setExposure(config.exposure);
    setToneCurve((ToneCurve)config.curve);
    setToeStrength(config.toeStrength);
    setToeLength(config.toeLength);
    setShoulderStrength(config.shoulderStrength);
    setShoulderLength(config.shoulderLength);
    setShoulderAngle(config.shoulderAngle);
    setGamma(config.gamma);
}

void ToneMapAndResample::run(const RenderContextPointer& renderContext, const Input& input, Output& output) {
    assert(renderContext->args);
    assert(renderContext->args->hasViewFrustum());

    RenderArgs* args = renderContext->args;

    auto lightingBuffer = input.get0()->getRenderBuffer(0);
    auto destinationFramebuffer = input.get1();

    if (!destinationFramebuffer) {
        destinationFramebuffer = args->_blitFramebuffer;
    }

    if (!lightingBuffer || !destinationFramebuffer) {
        return;
    }

    if (!_pipeline) {
        init();
    }

    const auto bufferSize = destinationFramebuffer->getSize();

    auto srcBufferSize = glm::ivec2(lightingBuffer->getDimensions());

    glm::ivec4 destViewport{ 0, 0, bufferSize.x, bufferSize.y };

    auto& params = _parametersBuffer.get<Parameters>();

    gpu::doInBatch("Resample::run", args->_context, [&](gpu::Batch& batch) {
        batch.enableStereo(false);
        batch.setFramebuffer(destinationFramebuffer);

        batch.setViewportTransform(destViewport);
        batch.setProjectionTransform(glm::mat4());
        batch.resetViewTransform();
        batch.setPipeline(params._toneCurve == 4 ?
            (args->_renderMode == RenderArgs::MIRROR_RENDER_MODE ? _piecewisePipeline : _piecewiseMirrorPipeline) :
            (args->_renderMode == RenderArgs::MIRROR_RENDER_MODE ? _mirrorPipeline : _pipeline));
            

        batch.setModelTransform(gpu::Framebuffer::evalSubregionTexcoordTransform(srcBufferSize, args->_viewport));
        batch.setUniformBuffer(render_utils::slot::buffer::ToneMappingParams, _parametersBuffer);
        batch.setResourceTexture(render_utils::slot::texture::ToneMappingColor, lightingBuffer);
        batch.draw(gpu::TRIANGLE_STRIP, 4);
    });

    // Set full final viewport
    args->_viewport = destViewport;

    output = destinationFramebuffer;
}
