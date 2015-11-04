//
//  GLBuffer.h
//  libraries/gpu/src/gpu/gl
//
//  Created by Sam Gateau on 11/01/2015.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#ifndef hifi_gpu_gl_GLBuffer_h
#define hifi_gpu_gl_GLBuffer_h

#include <stdint.h>
#include <vector>

#include <gl/Config.h>

namespace gl {



// This code is directly inpired and applied from John Mc Donalds & Cass Everit Presentation:
// Beyond Porting: How Modern OpenGL Can Radically Reduce Driver Overhead at Steam Dev 2014
// https://developer.nvidia.com/content/how-modern-opengl-can-radically-reduce-driver-overhead-0

// BufferLockManager is a class responsible for bookeeping the life of GLSync objects protecting
// ranges of a buffer currently in use by the gpu in order to avoid read/write collision
class BufferLockManager {
public:
    class Range {
    public:
        size_t start{ 0 };
        size_t size{ 0 };
        Range(size_t byteStart, size_t byteSize) : start(byteStart), size(byteSize) {}

        bool overlaps(const Range& rhs) const {
            return (start < (rhs.start + rhs.size))
            && (rhs.start < (start + size));
        }
    };

    class Lock {
    public:
        Range range;
        GLsync syncObj;
    };

    BufferLockManager(bool cpuUpdates);
    ~BufferLockManager();
    
    void waitLockedRange(size_t lockBeginBytes, size_t lockLength);
    void lockRange(size_t _lockBeginBytes, size_t _lockLength);

private:
    void wait(GLsync* syncObj);
    void cleanup(Lock& bufferLock);
    
    std::vector<Lock> _bufferLocks;

    // Whether it's the CPU (true) that updates, or the GPU (false)
    bool _cpuUpdates;
};

class Buffer {
public:
    enum class Usage {
        DynamicWrite,
        PersistentMapDynamicWrite,
    };

    Buffer(bool cpuUpdates) : _lockManager(cpuUpdates) {}
    ~Buffer() { destroy(); }

    bool create(Usage Usage, GLenum target, GLuint numAtoms, size_t atomSize);
    void destroy();

    // #1: Before accessing an atom range, call waitLockedRange to guarantee it is available for access
    void waitLockedRange(GLuint lockOffset, GLuint lockLength) { _lockManager.waitLockedRange(lockOffset, lockLength); }
    // #2: Map the atom range returning a mapped pointer to it
    void* mapRange(GLuint atomOffset, GLuint atomLength);
    // #3: Once done using the pointer, call unmap. Note that this is a no op if the usage is "PersistentMapDynamicWrite"
    void unmap();
    // #4 Finally the gl commands using that atom range have been called, lock it
    void lockRange(GLuint lockOffset, GLuint lockLength) { _lockManager.lockRange(lockOffset, lockLength); }


    void bindBuffer() { glBindBuffer(_target, _name); }
    void bindBufferBase(GLuint index) { glBindBufferBase(_target, index, _name); }
    void bindBufferRange(GLuint index, GLuint atomOffset, GLuint atomLength) {
        glBindBufferRange(_target, index, _name, atomOffset * _atomStride, atomLength * _atomStride);
    }

    GLuint getNumAtoms() const { return _numAtoms; }
    GLsizeiptr getAtomStride() const { return _atomStride; }

    GLvoid* getMappedPointer() { return _mappedPointer; }

private:
    BufferLockManager _lockManager;
    GLvoid* _mappedPointer{ nullptr };
    GLuint _name{ 0 };
    GLenum _target{ GL_UNIFORM_BUFFER_BINDING };

    Usage _usage{ Usage::DynamicWrite };
    GLuint _numAtoms{ 0 };
    GLsizeiptr _atomStride{ 0 };
    GLsizeiptr _atomSize{ 0 };
    bool _mappingStarted{ false };

};

class CircularBuffer {
public:
    CircularBuffer(bool _cpuUpdates = true) :
        _buffer(_cpuUpdates)
    {}

    bool create(Buffer::Usage Usage, GLenum target, GLuint numAtoms, size_t atomSize) {
        _head = 0;
        return _buffer.create(Usage, target, numAtoms, atomSize);
    }
    
    void destroy() {
        _buffer.destroy();
        _head = 0;
    }
    
    void* map(GLuint numAtoms) {
        if (numAtoms > _buffer.getNumAtoms()) {
            return nullptr;
        }

        GLsizeiptr lockStart = _head;
        if (lockStart + numAtoms > _buffer.getNumAtoms()) {
            // Need to wrap here.
            lockStart = 0;
            _head = 0;
        }
        _mappedLength = numAtoms;
        _buffer.waitLockedRange(lockStart, numAtoms);
        return _buffer.mapRange(lockStart, numAtoms);
    }
    
    void unmap() {
        _buffer.unmap();
    }

    template <class Atom>
    void upload(const Atom* data, GLuint numAtoms) {
        void* pointer = map(numAtoms);
        auto atomStride = _buffer.getAtomStride();
        for (int i = 0; i< numAtoms; i++) {
            auto newPointer = reinterpret_cast<size_t>(pointer) + (i * atomStride);
            memcpy((void*)newPointer, data + i, sizeof(Atom));
        }
        unmap();
    }
    
    void onUsageComplete() {
        if (_mappedLength) {
            _buffer.lockRange(_head, _mappedLength);
            _head = (_head + _mappedLength) % _buffer.getNumAtoms();
        }
        _mappedLength = 0;
    }
    
    void bindBuffer() { _buffer.bindBuffer(); }
    void bindBufferBase(GLuint index) { _buffer.bindBufferBase(index); }

    void bindBufferRangeCurrent(GLuint index, GLuint numAtoms) { _buffer.bindBufferRange(index, _head, numAtoms); }
    void bindBufferRange(GLuint index, GLuint atomOffset, GLuint numAtoms) {
        _buffer.bindBufferRange(index, _head + atomOffset, numAtoms);
    }
    
    GLsizeiptr getHead() const { return _head; }
    GLsizeiptr getSize() const { return _buffer.getNumAtoms() * _buffer.getAtomStride(); }
    
private:
    Buffer _buffer;
    GLuint _head{ 0 };
    GLuint _mappedLength{ 0 };
};

}

#endif