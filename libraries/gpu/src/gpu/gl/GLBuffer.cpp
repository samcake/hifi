//
//  GLBuffer.cpp
//  libraries/gpu/src/gpu/gl
//
//  Created by Sam Gateau on 11/01/2015.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "GLBuffer.h"

namespace gl {

GLuint64 ONE_SECOND_IN_NANO_SECONDS = 1000000000;

BufferLockManager::BufferLockManager(bool cpuUpdates) :
    _cpuUpdates(cpuUpdates)
{
}

BufferLockManager::~BufferLockManager() {
    for (auto& it : _bufferLocks) {
        cleanup(it);
    }
    _bufferLocks.clear();
}

void BufferLockManager::waitLockedRange(size_t lockBeginBytes, size_t lockLength) {
    Range testRange = { lockBeginBytes, lockLength };
    std::vector<Lock> swapLocks;
    for (auto it : _bufferLocks) {
        if (testRange.overlaps(it.range)) {
            wait(&it.syncObj);
            cleanup(it);
        } else {
            swapLocks.push_back(it);
        }
    }
    
    _bufferLocks.swap(swapLocks);
}

void BufferLockManager::lockRange(size_t _lockBeginBytes, size_t _lockLength) {
    Range newRange = { _lockBeginBytes, _lockLength };
    GLsync syncName = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    Lock newLock = { newRange, syncName };
    
    _bufferLocks.push_back(newLock);
}

void BufferLockManager::wait(GLsync* syncObj) {
    if  (_cpuUpdates) {
        GLbitfield waitFlags = 0;
        GLuint64 waitDuration = 0;
        while (1) {
            GLenum waitRet = glClientWaitSync(*syncObj, waitFlags, waitDuration);
            if (waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED) {
                return;
            }
            
            if (waitRet == GL_WAIT_FAILED) {
               // FIXME: Not sure what to do here. Probably raise an exception or something;
                return;
            }
            
            // After the first time, need to start flushing, and wait for a looong time.
            waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
            waitDuration = ONE_SECOND_IN_NANO_SECONDS;
        }
    } else {
        glWaitSync(*syncObj, 0, GL_TIMEOUT_IGNORED);
    }
}

void BufferLockManager::cleanup(Lock& bufferLock) {
    glDeleteSync(bufferLock.syncObj);
}


bool Buffer::create(Usage Usage, GLenum target, GLuint numAtoms, size_t atomSize) {
    if (_mappedPointer) {
        destroy();
    }
        
    _usage = Usage;
    _target = target;
    _atomStride = 0;
    _atomSize = (atomSize < 1 ? 1 : atomSize);
    GLint bindingBufferAlignment = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &bindingBufferAlignment);
    while(_atomStride < _atomSize) {
        _atomStride += bindingBufferAlignment;
    }
    _numAtoms = numAtoms;

    switch (_usage) {
        case Buffer::Usage::DynamicWrite: {
            glGenBuffers(1, &_glObject);
            glBindBuffer(_target, _glObject);
            glBufferData(_target, _atomStride * _numAtoms, nullptr, GL_DYNAMIC_DRAW);
            break;
        }
        case Buffer::Usage::PersistentMapDynamicWrite: {
            glGenBuffers(1, &_glObject);
            glBindBuffer(_target, _glObject);
            const GLbitfield mapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
            const GLbitfield createFlags = mapFlags | GL_DYNAMIC_STORAGE_BIT;
            glBufferStorage(_target, _atomStride * _numAtoms, nullptr, createFlags);
            _mappedPointer = (glMapBufferRange(_target, 0, _atomStride * _numAtoms, mapFlags));
            if (!_mappedPointer) {
                return false;
            }
            break;
        }
    };
    return true;
}

void Buffer::destroy() {
    switch (_usage) {
        case Buffer::Usage::DynamicWrite: {
            glDeleteBuffers(1, &_glObject);
            break;
        }
        case Buffer::Usage::PersistentMapDynamicWrite: {
            glBindBuffer(_target, _glObject);
            glUnmapBuffer(_target);
            glDeleteBuffers(1, &_glObject);
            _mappedPointer = nullptr;
            break;
        }
    };

    _glObject = 0;
    _numAtoms = 0;
    _atomSize = 0;
    _atomStride = 0;
}

void* Buffer::mapRange(GLuint atomOffset, GLuint atomLength) {
    switch (_usage) {
        case Buffer::Usage::DynamicWrite: {
            _mappingStarted = true;
            GLsizeiptr rangeOffset = atomOffset * _atomStride;
            GLsizeiptr rangeLength = atomLength * _atomStride;
            glBindBuffer(GL_COPY_WRITE_BUFFER, _glObject);
            auto pointer = (glMapBufferRange(GL_COPY_WRITE_BUFFER, rangeOffset, rangeLength, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
            return pointer;
            break;
        }
        case Buffer::Usage::PersistentMapDynamicWrite: {
            GLsizeiptr rangeOffset = atomOffset * _atomStride;
            return (void*) (reinterpret_cast<GLsizeiptr> (_mappedPointer)+rangeOffset);
            break;
        }
    }
    return nullptr;
}
    
void Buffer::unmap() {
    switch (_usage) {
        case Usage::DynamicWrite: {
            if (!_mappingStarted) {
                _mappingStarted = false;
            } else {
                _mappingStarted = false;
                glUnmapBuffer(GL_COPY_WRITE_BUFFER);
                glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            }
            break;
        }
        case Usage::PersistentMapDynamicWrite: {
            break;
        }
    }
}

}
