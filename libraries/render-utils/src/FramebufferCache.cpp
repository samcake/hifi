//
//  FramebufferCache.cpp
//  interface/src/renderer
//
//  Created by Andrzej Kapolka on 8/6/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "FramebufferCache.h"

#include <glm/glm.hpp>
#include <gpu/Format.h>
#include <gpu/Framebuffer.h>

#include "RenderUtilsLogging.h"

void FramebufferCache::setFrameBufferSize(const glm::uvec2& frameBufferSize, bool isStereo) {
    bool dirty = false;

    //If the size changed, we need to delete our FBOs
    if ((_frameBufferSize != frameBufferSize) || (_frameBufferStereo != isStereo)) {
        _frameBufferStereo = isStereo;
        _frameBufferSize = frameBufferSize;

        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cachedFramebuffers.clear();
        }
    }
}

void FramebufferCache::createPrimaryFramebuffer() {
    auto defaultSampler = gpu::Sampler(gpu::Sampler::FILTER_MIN_MAG_POINT);

    auto smoothSampler = gpu::Sampler(gpu::Sampler::FILTER_MIN_MAG_MIP_LINEAR);
}


gpu::FramebufferPointer FramebufferCache::getFramebuffer() {
    std::unique_lock<std::mutex> lock(_mutex);
    if (_cachedFramebuffers.empty()) {
        
        auto framebuffer = gpu::Framebuffer::create("cached");

        if (_frameBufferStereo) {
            auto colorTexture = gpu::TexturePointer(
            gpu::Texture::createRenderBufferArray(gpu::Element::COLOR_SRGBA_32, _frameBufferSize.x, _frameBufferSize.y, 2,
                                                        gpu::Texture::SINGLE_MIP,
                                                        gpu::Sampler(gpu::Sampler::FILTER_MIN_MAG_POINT)));
            colorTexture->setSource("Framebuffer::colorTexture");
            framebuffer->setRenderBuffer(0, colorTexture, gpu::TextureView::UNDEFINED_SUBRESOURCE);
        } else {
            auto colorTexture = gpu::TexturePointer(
                gpu::Texture::createRenderBuffer(gpu::Element::COLOR_SRGBA_32, _frameBufferSize.x, _frameBufferSize.y, 
                                                        gpu::Texture::SINGLE_MIP,
                                                        gpu::Sampler(gpu::Sampler::FILTER_MIN_MAG_POINT)));
            colorTexture->setSource("Framebuffer::colorTexture");
            framebuffer->setRenderBuffer(0, colorTexture);
        }

        _cachedFramebuffers.push_back(gpu::FramebufferPointer(framebuffer));
    }
    gpu::FramebufferPointer result = _cachedFramebuffers.front();
    _cachedFramebuffers.pop_front();
    return result;
}

void FramebufferCache::releaseFramebuffer(const gpu::FramebufferPointer& framebuffer) {
    std::unique_lock<std::mutex> lock(_mutex);
    if (framebuffer->getSize() == _frameBufferSize) {
        _cachedFramebuffers.push_back(framebuffer);
    }
}
