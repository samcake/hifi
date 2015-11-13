//
//  GLBackendBuffer.cpp
//  libraries/gpu/src/gpu
//
//  Created by Sam Gateau on 3/8/2015.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "GLBackendShared.h"

using namespace gpu;

GLBackend::GLBuffer::~GLBuffer() {
    if (_buffer != 0) {
        glDeleteBuffers(1, &_buffer);
    }
    if (_ringBuffer) {
        _ringBuffer->destroy();
    }
}

GLBackend::GLBuffer* GLBackend::syncGPUObject(const Buffer& buffer) {
    GLBuffer* object = Backend::getGPUObject<GLBackend::GLBuffer>(buffer);

    if (object && (object->_stamp == buffer.getSysmem().getStamp())) {
        return object;
    }

    // need to have a gpu object?
    if (!object) {
        object = new GLBuffer();
        Backend::setGPUObject(buffer, object);
    }

    if (buffer.isDynamic()) {

        // Create the ringBuffer if needed or recreate it if it actually grew in size
        if (!object->_ringBuffer || (buffer.getSysmem().getSize() > object->_size)) {
            object->_ringBuffer = std::make_shared<gl::CircularBuffer>();
            object->_ringBuffer->create(gl::Buffer::Usage::PersistentMapDynamicWrite, GL_UNIFORM_BUFFER, 3, buffer.getSysmem().getSize());
        }

        // TIme to upload sysmem to the buffer
        object->_ringBuffer->upload(buffer.getSysmem().readData(), 1);
        object->_stamp = buffer.getSysmem().getStamp();
        object->_size = buffer.getSysmem().getSize();

        (void) CHECK_GL_ERROR();
    } else {
        // Create the BO if needed
        if (object->_buffer == 0) {
            glGenBuffers(1, &object->_buffer);
            (void) CHECK_GL_ERROR();
        }

        // Now let's update the content of the bo with the sysmem version
        // The Buffer is  not "dynamic" and so supposely beeing updated rarely, we just do a full update of BufferData
        glBindBuffer(GL_ARRAY_BUFFER, object->_buffer);
        glBufferData(GL_ARRAY_BUFFER, buffer.getSysmem().getSize(), buffer.getSysmem().readData(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        object->_stamp = buffer.getSysmem().getStamp();
        object->_size = buffer.getSysmem().getSize();
        //}
        (void) CHECK_GL_ERROR();
    }

    object->_syncedAtBatch = _batch.batchNum;

    return object;
}



GLuint GLBackend::getBufferID(const Buffer& buffer) {
    GLBuffer* bo = GLBackend::syncGPUObject(buffer);
    if (bo) {
        return bo->getID();
    } else {
        return 0;
    }
}

GLuint GLBackend::GLBuffer::getID() {
    if (_buffer) {
        return _buffer;
    } else if (_ringBuffer) {
        return _ringBuffer->getBuffer().getGLObject();
    }
}

GLsizeiptr GLBackend::GLBuffer::getHeadOffset() {
    if (_buffer) {
        return 0;
    } else if (_ringBuffer) {
        return _ringBuffer->getHeadOffset();
    }
}

bool GLBackend::GLBuffer::bindBuffer(GLenum target) {
    if (_buffer) {
        glBindBuffer(target, _buffer);
        return true;
    } else if (_ringBuffer) {
        _ringBuffer->bindBuffer(target);
        return true;
    }

    return true;
}

bool GLBackend::GLBuffer::bindBufferRange(GLenum target, GLuint index, GLuint offset, GLuint length) {
    if (_buffer) {
        glBindBufferRange(target, index, _buffer, offset, length);
        return true;
    } else if (_ringBuffer) {
        if (offset == 0 && length == _size) {
            _ringBuffer->bindBufferRangeCurrent(target, index, 1);
            return true;
        }
    }

    return true;
}