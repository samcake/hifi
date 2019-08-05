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
    float twoPowExp = pow(2.0, exposure);
    if (params._twoPowExposure != twoPowExp) {
        _parametersBuffer.edit<Parameters>()._twoPowExposure = twoPowExp;
    }
}

void ToneMapAndResample::setToneCurve(ToneCurve curve) {
    auto& params = _parametersBuffer.get<Parameters>();
    if (params._toneCurve != (int)curve) {
        _parametersBuffer.edit<Parameters>()._toneCurve = (int)curve;
    }
}
void ToneMapAndResample::setToeStrength(float strength) {
    if (userParams.m_toeStrength != strength) {
        userParams.m_toeStrength = strength;
        _dirty = true;
    }
}

void ToneMapAndResample::setToeLength(float length) {
    if (userParams.m_toeLength != length) {
        userParams.m_toeLength = length;
        _dirty = true;
    }
}

void ToneMapAndResample::setShoulderStrength(float strength) {
    if (userParams.m_shoulderStrength != strength) {
        userParams.m_shoulderStrength = strength;
        _dirty = true;
    }
}

void ToneMapAndResample::setShoulderLength(float length) {
    if (userParams.m_shoulderLength != length) {
        userParams.m_shoulderLength = length;
        _dirty = true;
    }
}

void ToneMapAndResample::setShoulderAngle(float angle) {
    if (userParams.m_shoulderAngle != angle) {
        userParams.m_shoulderAngle = angle;
        _dirty = true;
    }
}

void ToneMapAndResample::setGamma(float gamma) {
    if (userParams.m_gamma != gamma) {
        userParams.m_gamma = gamma;
        _dirty = true;
    }
}

void ToneMapAndResample::setCurveParams(FullCurve curve) {
    auto& params = _parametersBuffer.edit<Parameters>();

    CurveSegment toe = m_segments[0];
    CurveSegment linear = m_segments[1];
    CurveSegment shoulder = m_segments[2];

    _parametersBuffer.edit<Parameters>()._shoulderOffsetX = shoulder.m_offsetX;
    _parametersBuffer.edit<Parameters>()._shoulderOffsetY = shoulder.m_offsetY;
    _parametersBuffer.edit<Parameters>()._shoulderLnA = shoulder.m_lnA;
    _parametersBuffer.edit<Parameters>()._shoulderB = shoulder.m_B;
    _parametersBuffer.edit<Parameters>()._toeLnA = toe.m_lnA;
    _parametersBuffer.edit<Parameters>()._toeB = toe.m_B;
    _parametersBuffer.edit<Parameters>()._linearLnA = linear.m_lnA;
    _parametersBuffer.edit<Parameters>()._linearB = linear.m_B;
    _parametersBuffer.edit<Parameters>()._linearOffsetX = linear.m_offsetX;
    _parametersBuffer.edit<Parameters>()._fullCurveW = curve.m_W;
    _parametersBuffer.edit<Parameters>()._fullCurveInvW = curve.m_invW;
    _parametersBuffer.edit<Parameters>()._fullCurveX0 = curve.m_x0;
    _parametersBuffer.edit<Parameters>()._fullCurveY0 = curve.m_y0;
    _parametersBuffer.edit<Parameters>()._fullCurveX1 = curve.m_x1;
    _parametersBuffer.edit<Parameters>()._fullCurveY1 = curve.m_y1;
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
