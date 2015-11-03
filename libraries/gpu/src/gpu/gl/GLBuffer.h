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

struct BufferRange {
    size_t start{ 0 };
    size_t size{ 0 };
    BufferRange(size_t byteStart, size_t byteSize) : start(byteStart), size(byteSize) {}
    
    bool overlaps(const BufferRange& rhs) const {
        return (start < (rhs.start + rhs.size))
        && (rhs.start < (start + size));
    }
};

struct BufferLock {
    BufferRange range;
    GLsync syncObj;
};

class BufferLockManager {
public:
    BufferLockManager(bool cpuUpdates);
    ~BufferLockManager();
    
    void waitForLockedRange(size_t lockBeginBytes, size_t lockLength);
    void lockRange(size_t _lockBeginBytes, size_t _lockLength);
    
private:
    void wait(GLsync* syncObj);
    void cleanup(BufferLock& bufferLock);
    
    std::vector<BufferLock> _bufferLocks;
    
    // Whether it's the CPU (true) that updates, or the GPU (false)
    bool _cpuUpdates;
};



enum class BufferStorage
{
    SystemMemory,
    SynchMappedBuffer,
    PersistentlyMappedBuffer
};

template <typename Atom>
class Buffer {
public:
    Buffer(bool cpuUpdates) :
        _lockManager(cpuUpdates),
        _bufferContents(),
        _name(),
        _target(),
        _bufferStorage(BufferStorage::SystemMemory),
        _atomStride()
    { }
    
    ~Buffer() {
        destroy();
    }
    
    bool create(GLsizeiptr uboAlignment, BufferStorage storage, GLenum target, GLuint atomCount, GLbitfield createFlags, GLbitfield mapFlags) {
        if (_bufferContents) {
            destroy();
        }
        
        _bufferStorage = storage;
        _target = target;
        _atomStride = 0;
        while(_atomStride < sizeof(Atom)) {
            _atomStride += uboAlignment;
        }
        
        switch (_bufferStorage) {
            case BufferStorage::SystemMemory: {
                _bufferContents = new Atom[atomCount];
                break;
            }
            case BufferStorage::SynchMappedBuffer: {
                glGenBuffers(1, &_name);
                glBindBuffer(_target, _name);
                glBufferData(_target, _atomStride * atomCount, nullptr, GL_STATIC_DRAW);
                break;
            }
            case BufferStorage::PersistentlyMappedBuffer: {
                // This code currently doesn't care about the alignment of the returned memory. This could potentially
                // cause a crash, but since implementations are likely to return us memory that is at lest aligned
                // on a 64-byte boundary we're okay with this for now.
                // A robust implementation would ensure that the memory returned had enough slop that it could deal
                // with it's own alignment issues, at least. That's more work than I want to do right this second.
                
                glGenBuffers(1, &_name);
                glBindBuffer(_target, _name);
                glBufferStorage(_target, sizeof(Atom) * atomCount, nullptr, createFlags);
                _bufferContents = reinterpret_cast<Atom*>(glMapBufferRange(_target, 0, sizeof(Atom) * atomCount, mapFlags));
                if (!_bufferContents) {
                  //  console::warn("glMapBufferRange failed, probable bug.");
                    return false;
                }
                break;
            }
                
            default: {
              //  console::error("Error, need to update Buffer::Create with logic for _bufferStorage = %d", _bufferStorage);
                break;
            }
        };
        
        return true;
    }
    
    // Called by dtor, must be non-virtual.
    void destroy() {
        switch (_bufferStorage) {
            case BufferStorage::SystemMemory: {
                delete[] _bufferContents;
                _bufferContents = nullptr;
                break;
            }
            case BufferStorage::SynchMappedBuffer: {
                glDeleteBuffers(1, &_name);
                _bufferContents = nullptr;
                _name = 0;
                break;
            }
            case BufferStorage::PersistentlyMappedBuffer: {
                glBindBuffer(_target, _name);
                glUnmapBuffer(_target);
                glDeleteBuffers(1, &_name);
                
                _bufferContents = nullptr;
                _name = 0;
                break;
            }
                
            default: {
              //  console::error("Error, need to update Buffer::Create with logic for _bufferStorage = %d", _bufferStorage);
                break;
            }
        };
    }
    
    void* mapRange(size_t lockBegin, size_t lockLength) {
        switch (_bufferStorage) {
            case BufferStorage::SystemMemory:
            case BufferStorage::PersistentlyMappedBuffer: {
                return &_bufferContents[lockBegin];
                break;
            }
            case BufferStorage::SynchMappedBuffer: {
                _mappingStarted = true;
                GLsizeiptr rangeOffset = lockBegin * _atomStride;
                GLsizeiptr rangeLength = lockLength * _atomStride;
                auto pointer = (glMapBufferRange(_target, rangeOffset, rangeLength, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
                if (!pointer) {
                    auto error = glGetError();
                    if ( error != GL_NO_ERROR) {
                        return pointer;
                    }
                }
                return pointer;
                break;
            }
        }
        return nullptr;
    }
    
    void unmap() {
        switch (_bufferStorage) {
            case BufferStorage::SystemMemory:
            case BufferStorage::PersistentlyMappedBuffer: {
                break;
            }
            case BufferStorage::SynchMappedBuffer: {
                if (!_mappingStarted) {
                    _mappingStarted = false;
                } else {
                    _mappingStarted = false;
                    glUnmapBuffer(_target);
                }
                break;
            }
        }
    }
    void waitForLockedRange(size_t lockBegin, size_t lockLength) { _lockManager.waitForLockedRange(lockBegin, lockLength); }
    Atom* getContents() { return _bufferContents; }
    void lockRange(size_t lockBegin, size_t lockLength) { _lockManager.lockRange(lockBegin, lockLength); }
    
    
    void bindBuffer() { glBindBuffer(_target, _name); }
    void bindBufferBase(GLuint index) { glBindBufferBase(_target, index, _name); }
    void bindBufferRange(GLuint index, GLsizeiptr head, GLsizeiptr atomCount) {
        glBindBufferRange(_target, index, _name, head * _atomStride, atomCount * _atomStride);
    }

    size_t getByteSize(uint32_t numAtoms) const { return numAtoms * _atomStride; }
    
    GLsizeiptr getAtomStride() const { return _atomStride; }
    
private:
    BufferLockManager _lockManager;
    Atom* _bufferContents;
    GLuint _name;
    GLenum _target;
                                                
    BufferStorage _bufferStorage;
    GLsizeiptr _atomStride{ 0 };
    bool _mappingStarted{ false };
};


template <typename Atom>
class CircularBuffer {
public:
    CircularBuffer(bool _cpuUpdates = true) :
        _buffer(_cpuUpdates)
    {}
    
    bool create(GLsizeiptr uboAlignment, BufferStorage storage, GLenum target, GLuint atomCount, GLbitfield createFlags, GLbitfield mapFlags) {
        _numAtoms = atomCount;
        _head = 0;
        
        return _buffer.create(uboAlignment, storage, target, atomCount, createFlags, mapFlags);
    }
    
    void Destroy() {
        _buffer.Destroy();
        
        _numAtoms = 0;
        _head = 0;
    }
    
    void* map(GLsizeiptr atomCount) {
        if (atomCount > _numAtoms) {
 //           console::error("Requested an update of size %d for a buffer of size %d atoms.", atomCount, mSizeAtoms);
        }
        
        GLsizeiptr lockStart = _head;
        
        if (lockStart + atomCount > _numAtoms) {
            // Need to wrap here.
            lockStart = 0;
            _head = 0;
        }
        _mappedLength = atomCount;
        _buffer.waitForLockedRange(lockStart, atomCount);
        return _buffer.mapRange(lockStart, atomCount);
    }
    
    void unmap() {
        _buffer.unmap();
    }
    
    void upload(const Atom* data, GLsizeiptr numAtoms) {
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
            _head = (_head + _mappedLength) % _numAtoms;
        }
        _mappedLength = 0;
    }
    
    void bindBuffer() { _buffer.bindBuffer(); }
    void bindBufferBase(GLuint index) { _buffer.bindBufferBase(index); }
    void bindBufferRangeCurrent(GLuint index, GLsizeiptr atomCount) { _buffer.bindBufferRange(index, _head, atomCount); }
    void bindBufferRange(GLuint index, GLsizeiptr atomOffset, GLsizeiptr atomCount) {
        _buffer.bindBufferRange(index, _head + atomOffset, atomCount);
    }
    
    GLsizeiptr getHead() const { return _head; }
    void* getHeadOffset() const { return (void*)(_head * sizeof(Atom)); }
    GLsizeiptr getSize() const { return _buffer.getSize(); }
    
private:
    Buffer<Atom> _buffer;
    
    GLsizeiptr _head{ 0 };
    GLsizeiptr _mappedLength{ 0 };
    // TODO: Maybe this should be in the Buffer class?
    GLsizeiptr _numAtoms{ 0 };
};

}

#endif