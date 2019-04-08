//
//  GL41BackendTexture.cpp
//  libraries/gpu/src/gpu
//
//  Created by Sam Gateau on 1/19/2015.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "GL41Backend.h"

#include <QtGui/QImage>

#include <gpu/gl/GLFramebuffer.h>
#include <gpu/gl/GLTexture.h>

namespace gpu { namespace gl41 { 

class GL41Framebuffer : public gl::GLFramebuffer {
    using Parent = gl::GLFramebuffer;
    static GLuint allocate() {
        GLuint result;
        glGenFramebuffers(1, &result);
        return result;
    }
public:
    void update() override {
        GLint currentFBO = -1;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        gl::GLTexture* gltexture = nullptr;
        TexturePointer surface;
        if (_gpuObject.getColorStamps() != _colorStamps) {
            if (_gpuObject.hasColor()) {
                _colorBuffers.clear();
                static const GLenum colorAttachments[] = {
                    GL_COLOR_ATTACHMENT0,
                    GL_COLOR_ATTACHMENT1,
                    GL_COLOR_ATTACHMENT2,
                    GL_COLOR_ATTACHMENT3,
                    GL_COLOR_ATTACHMENT4,
                    GL_COLOR_ATTACHMENT5,
                    GL_COLOR_ATTACHMENT6,
                    GL_COLOR_ATTACHMENT7,
                    GL_COLOR_ATTACHMENT8,
                    GL_COLOR_ATTACHMENT9,
                    GL_COLOR_ATTACHMENT10,
                    GL_COLOR_ATTACHMENT11,
                    GL_COLOR_ATTACHMENT12,
                    GL_COLOR_ATTACHMENT13,
                    GL_COLOR_ATTACHMENT14,
                    GL_COLOR_ATTACHMENT15 };

                int unit = 0;
                auto backend = _backend.lock();
                for (auto& b : _gpuObject.getRenderBuffers()) {
                    surface = b._texture;
                    auto surfaceSubresource = b._subresource;
                    if (surface) {
                        Q_ASSERT(TextureUsageType::RENDERBUFFER == surface->getUsageType());
                        gltexture = backend->syncGPUObject(surface); 
                    } else {
                        gltexture = nullptr;
                    }

                    if (gltexture) {
                         if (surfaceSubresource != gpu::TextureView::UNDEFINED_SUBRESOURCE) {
                            switch (gltexture->_target) {
                                case GL_TEXTURE_2D:
                                    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_MULTISAMPLE:
                                    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_ARRAY:
                                    glFramebufferTextureLayer(GL_FRAMEBUFFER, colorAttachments[unit], gltexture->_texture, 0,
                                                                   (GLuint)surfaceSubresource);
                                    break;
                                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                                    glFramebufferTextureLayer(GL_FRAMEBUFFER, colorAttachments[unit], gltexture->_texture, 0,
                                                                   (GLuint)surfaceSubresource);
                                    break;
                                default:
                                    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachments[unit], 0, 0);
                                    // not a valid path
                                    break;
                            }
                        } else {
                            switch (gltexture->_target) {
                                case GL_TEXTURE_2D:
                                    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_MULTISAMPLE:
                                    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_ARRAY:
                                    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                                    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                default:
                                    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachments[unit], 0, 0);
                                    // not a valid path
                                    break;
                            }
                        }

                        _colorBuffers.push_back(colorAttachments[unit]);
                    } else {
                        glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachments[unit], GL_TEXTURE_2D, 0, 0);
                    }
                    unit++;
                }
            }
            _colorStamps = _gpuObject.getColorStamps();
        }

        GLenum attachement = GL_DEPTH_STENCIL_ATTACHMENT;
        if (!_gpuObject.hasStencil()) {
            attachement = GL_DEPTH_ATTACHMENT;
        } else if (!_gpuObject.hasDepth()) {
            attachement = GL_STENCIL_ATTACHMENT;
        }

        if (_gpuObject.getDepthStamp() != _depthStamp) {
            auto backend = _backend.lock();
            auto surfaceSubresource = _gpuObject.getDepthStencilBufferSubresource();
            auto surface = _gpuObject.getDepthStencilBuffer();
            if (_gpuObject.hasDepthStencil() && surface) {
                Q_ASSERT(TextureUsageType::RENDERBUFFER == surface->getUsageType());
                gltexture = backend->syncGPUObject(surface);
            }

            if (gltexture) {
                if (surfaceSubresource != gpu::TextureView::UNDEFINED_SUBRESOURCE) {
                    switch (gltexture->_target) {
                        case GL_TEXTURE_2D:
                            glFramebufferTexture(GL_FRAMEBUFFER, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_MULTISAMPLE:
                            glFramebufferTexture(GL_FRAMEBUFFER, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_ARRAY:
                            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachement, gltexture->_texture, 0, (GLuint)surfaceSubresource);
                            break;
                        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachement, gltexture->_texture, 0, (GLuint)surfaceSubresource);
                            break;
                        default:
                            glFramebufferTexture(GL_FRAMEBUFFER, attachement, 0, 0);
                            // not a valid path
                            break;
                    }
                } else {
                    switch (gltexture->_target) {
                        case GL_TEXTURE_2D:
                            glFramebufferTexture(GL_FRAMEBUFFER, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_MULTISAMPLE:
                            glFramebufferTexture(GL_FRAMEBUFFER, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_ARRAY:
                            glFramebufferTexture(GL_FRAMEBUFFER, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                            glFramebufferTexture(GL_FRAMEBUFFER, attachement, gltexture->_texture, 0);
                            break;
                        default:
                            glFramebufferTexture(GL_FRAMEBUFFER, attachement, 0, 0);
                            // not a valid path
                            break;
                    }
                }
            } else {
                glFramebufferTexture2D(GL_FRAMEBUFFER, attachement, GL_TEXTURE_2D, 0, 0);
            }
            _depthStamp = _gpuObject.getDepthStamp();
        }


        // Last but not least, define where we draw
        if (!_colorBuffers.empty()) {
            glDrawBuffers((GLsizei)_colorBuffers.size(), _colorBuffers.data());
        } else {
            glDrawBuffer(GL_NONE);
        }

        // Now check for completness
        _status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        // restore the current framebuffer
        if (currentFBO != -1) {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, currentFBO);
        }

        checkStatus();
    }


public:
    GL41Framebuffer(const std::weak_ptr<gl::GLBackend>& backend, const gpu::Framebuffer& framebuffer)
        : Parent(backend, framebuffer, allocate()) { }
};

gl::GLFramebuffer* GL41Backend::syncGPUObject(const Framebuffer& framebuffer) {
    return GL41Framebuffer::sync<GL41Framebuffer>(*this, framebuffer);
}

GLuint GL41Backend::getFramebufferID(const FramebufferPointer& framebuffer) {
    return framebuffer ? GL41Framebuffer::getId<GL41Framebuffer>(*this, *framebuffer) : 0;
}

void GL41Backend::do_blit(const Batch& batch, size_t paramOffset) {
    auto srcframebuffer = batch._framebuffers.get(batch._params[paramOffset]._uint);
    Vec4i srcvp;
    for (auto i = 0; i < 4; ++i) {
        srcvp[i] = batch._params[paramOffset + 1 + i]._int;
    }

    auto dstframebuffer = batch._framebuffers.get(batch._params[paramOffset + 5]._uint);
    Vec4i dstvp;
    for (auto i = 0; i < 4; ++i) {
        dstvp[i] = batch._params[paramOffset + 6 + i]._int;
    }

    auto fboSrc = syncGPUObject(*srcframebuffer);
    auto fboDst = syncGPUObject(*dstframebuffer);

    // Assign dest framebuffer if not bound already
    auto srcFbo = fboSrc->_id; //getFramebufferID(srcframebuffer);
    auto destFbo = fboDst->_id; //getFramebufferID(dstframebuffer);
    auto newDrawFbo = destFbo;

    // Assign dst to draw if not draw aleady
    if (_output._drawFBO != destFbo) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destFbo);
    }
    // always bind the read fbo
    glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFbo);

    // Blit!
    glBlitFramebuffer(srcvp.x, srcvp.y, srcvp.z, srcvp.w, 
        dstvp.x, dstvp.y, dstvp.z, dstvp.w,
        GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Deal with extra layers
    if (srcframebuffer->isLayered() && dstframebuffer->isLayered()) {
        auto numLayers = std::max(srcframebuffer->getNumLayers(), dstframebuffer->getNumLayers());
        std::vector<GLuint> layerBliters(2, 0);
        glCreateFramebuffers(2, layerBliters.data());

        auto srcSurface = fboSrc->_colorBuffers[0];
        auto dstSurface = fboDst->_colorBuffers[0];
        auto srcSurfaceTexture = fboSrc->_gpuObject.getRenderBuffers()[0]._texture;
        auto srcSurfaceTexturegl = syncGPUObject(srcSurfaceTexture);
        auto dstSurfaceTexture = fboDst->_gpuObject.getRenderBuffers()[0]._texture;
        auto dstSurfaceTexturegl = syncGPUObject(dstSurfaceTexture);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, layerBliters[0]);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, layerBliters[1]);

        for (int l = 1; l < numLayers; l++) {
            glFramebufferTextureLayer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, srcSurfaceTexturegl->_texture, 0, (GLuint)l);
            glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dstSurfaceTexturegl->_texture, 0, (GLuint)l);
            glBlitFramebuffer(
                srcvp.x, srcvp.y, srcvp.z, srcvp.w,
                dstvp.x, dstvp.y, dstvp.z, dstvp.w,
                GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        
        newDrawFbo = layerBliters[1];

        // Restore state
        glDeleteFramebuffers(2, layerBliters.data());
    }

    // Always clean the read fbo to 0
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // Restore draw fbo if changed
    if (_output._drawFBO != newDrawFbo) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _output._drawFBO);
    }

    (void) CHECK_GL_ERROR();
}


} }
