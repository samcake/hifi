//
//  GL45BackendTexture.cpp
//  libraries/gpu/src/gpu
//
//  Created by Sam Gateau on 1/19/2015.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "GL45Backend.h"
#include <gpu/gl/GLFramebuffer.h>
#include <gpu/gl/GLTexture.h>

#include <QtGui/QImage>

namespace gpu { namespace gl45 { 

class GL45Framebuffer : public gl::GLFramebuffer {
    using Parent = gl::GLFramebuffer;
    static GLuint allocate() {
        GLuint result;
        glCreateFramebuffers(1, &result);
        return result;
    }
public:
    void update() override {
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
                                    glNamedFramebufferTexture(_id, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_MULTISAMPLE:
                                    glNamedFramebufferTexture(_id, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_ARRAY:
                                    glNamedFramebufferTextureLayer(_id, colorAttachments[unit], gltexture->_texture, 0,
                                                                   (GLuint)surfaceSubresource);
                                    break;
                                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                                    glNamedFramebufferTextureLayer(_id, colorAttachments[unit], gltexture->_texture, 0,
                                                                   (GLuint)surfaceSubresource);
                                    break;
                                default:
                                    glNamedFramebufferTexture(_id, colorAttachments[unit], 0, 0);
                                    // not a valid path
                                    break;
                            }
                        } else {
                            switch (gltexture->_target) {
                                case GL_TEXTURE_2D:
                                    glNamedFramebufferTexture(_id, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_MULTISAMPLE:
                                    glNamedFramebufferTexture(_id, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_ARRAY:
                                    glNamedFramebufferTexture(_id, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                                    glNamedFramebufferTexture(_id, colorAttachments[unit], gltexture->_texture, 0);
                                    break;
                                default:
                                    glNamedFramebufferTexture(_id, colorAttachments[unit], 0, 0);
                                    // not a valid path
                                    break;
                            }
                        }

                        _colorBuffers.push_back(colorAttachments[unit]);
                    } else {
                        glNamedFramebufferTexture(_id, colorAttachments[unit], 0, 0);
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
            auto surface = _gpuObject.getDepthStencilBuffer();
            auto surfaceSubresource = _gpuObject.getDepthStencilBufferSubresource();
            auto backend = _backend.lock();
            if (_gpuObject.hasDepthStencil() && surface) {
                Q_ASSERT(TextureUsageType::RENDERBUFFER == surface->getUsageType());
                gltexture = backend->syncGPUObject(surface);
            }

            if (gltexture) {
                if (surfaceSubresource != gpu::TextureView::UNDEFINED_SUBRESOURCE) {
                    switch (gltexture->_target) {
                        case GL_TEXTURE_2D:
                            glNamedFramebufferTexture(_id, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_MULTISAMPLE:
                            glNamedFramebufferTexture(_id, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_ARRAY:
                            glNamedFramebufferTextureLayer(_id, attachement, gltexture->_texture, 0, (GLuint)surfaceSubresource);
                            break;
                        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                            glNamedFramebufferTextureLayer(_id, attachement, gltexture->_texture, 0, (GLuint)surfaceSubresource);
                            break;
                        default:
                            glNamedFramebufferTexture(_id, attachement, 0, 0);
                            // not a valid path
                            break;
                    }
                } else {
                    switch (gltexture->_target) {
                        case GL_TEXTURE_2D:
                            glNamedFramebufferTexture(_id, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_MULTISAMPLE:
                            glNamedFramebufferTexture(_id, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_ARRAY:
                            glNamedFramebufferTexture(_id, attachement, gltexture->_texture, 0);
                            break;
                        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
                            glNamedFramebufferTexture(_id, attachement, gltexture->_texture, 0);
                            break;
                        default:
                            glNamedFramebufferTexture(_id, attachement, 0, 0);
                            // not a valid path
                            break;
                    }
                }
            } else {
                glNamedFramebufferTexture(_id, attachement, 0, 0);
            }
            _depthStamp = _gpuObject.getDepthStamp();
        }

        // Last but not least, define where we draw
        if (!_colorBuffers.empty()) {
            glNamedFramebufferDrawBuffers(_id, (GLsizei)_colorBuffers.size(), _colorBuffers.data());
        } else {
            glNamedFramebufferDrawBuffer(_id, GL_NONE);
        }

        // Now check for completness
        _status = glCheckNamedFramebufferStatus(_id, GL_DRAW_FRAMEBUFFER);

        checkStatus();
    }


public:
    GL45Framebuffer(const std::weak_ptr<gl::GLBackend>& backend, const gpu::Framebuffer& framebuffer)
        : Parent(backend, framebuffer, allocate()) { }
};

gl::GLFramebuffer* GL45Backend::syncGPUObject(const Framebuffer& framebuffer) {
    return gl::GLFramebuffer::sync<GL45Framebuffer>(*this, framebuffer);
}

GLuint GL45Backend::getFramebufferID(const FramebufferPointer& framebuffer) {
    return framebuffer ? gl::GLFramebuffer::getId<GL45Framebuffer>(*this, *framebuffer) : 0;
}

void GL45Backend::do_blit(const Batch& batch, size_t paramOffset) {
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
    glBlitNamedFramebuffer(srcFbo, destFbo,
        srcvp.x, srcvp.y, srcvp.z, srcvp.w,
        dstvp.x, dstvp.y, dstvp.z, dstvp.w,
         GL_COLOR_BUFFER_BIT, GL_NEAREST);
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

        for (int l = 1; l < numLayers; l++) {
            glNamedFramebufferTextureLayer(layerBliters[0], GL_COLOR_ATTACHMENT0, srcSurfaceTexturegl->_texture, 0, (GLuint)l);
            glNamedFramebufferTextureLayer(layerBliters[1], GL_COLOR_ATTACHMENT0, dstSurfaceTexturegl->_texture, 0, (GLuint)l);
            glBlitNamedFramebuffer(layerBliters[0], layerBliters[1],
                srcvp.x, srcvp.y, srcvp.z, srcvp.w,
                dstvp.x, dstvp.y, dstvp.z, dstvp.w,
                GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
        
        glDeleteFramebuffers(2, layerBliters.data());
    }

    (void) CHECK_GL_ERROR();
}

} }
