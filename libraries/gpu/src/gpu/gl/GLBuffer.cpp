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

void BufferLockManager::waitForLockedRange(size_t lockBeginBytes, size_t lockLength) {
    BufferRange testRange = { lockBeginBytes, lockLength };
    std::vector<BufferLock> swapLocks;
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
    BufferRange newRange = { _lockBeginBytes, _lockLength };
    GLsync syncName = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    BufferLock newLock = { newRange, syncName };
    
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
               // assert(!"Not sure what to do here. Probably raise an exception or something.");
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

void BufferLockManager::cleanup(BufferLock& bufferLock) {
    glDeleteSync(bufferLock.syncObj);
}
