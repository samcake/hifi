//
//  ElementBuffer.h
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 4/19/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#ifndef hifi_crowd_ElementBuffer_h
#define hifi_crowd_ElementBuffer_h

#include "Crowd.h"

#include <gpu/Buffer.h>

namespace crowd {

    template <class T> class ElementBuffer : public gpu::BufferView {
    public:
        using value_type = T;
        using difference_type = int32_t;
        using size_type = uint32_t;
        using reference = T&;
        using const_reference = const T&;

        template <class U> static gpu::BufferPointer makeBuffer(size_type numElements = 0, const U* elements = nullptr) {
            return std::make_shared<gpu::Buffer>(sizeof(U) * numElements, (const gpu::Byte*) elements, sizeof(U));
        }
        virtual ~ElementBuffer<T>() {};
        ElementBuffer<T>() : gpu::BufferView(makeBuffer<T>()) {}
        ElementBuffer<T>(size_type capacity) : gpu::BufferView(makeBuffer<T>(capacity)) {}

        // std::vector Capacity:
        size_type size() const {
            return _numElements;
        }

        void resize(size_type n, value_type val = value_type()) {
            auto newByteSize = _buffer->resize(n * _stride);
            _numElements = n;
            assert((newByteSize / _stride) <= n);
        }

        size_type capacity() const {
            return (size_type) (_size / _stride);
        }

        bool empty() const {
            return !size();
        }

        void reserve(size_type n) {
            auto newByteSize = _buffer->resize(n * _stride);
            assert((newByteSize / _stride) >= n);
        }

        // std::vector Element access:
        T& edit(const Index index) {
            return BufferView::edit<T>(index);
        }
        const T& get(const Index index) const {
            return BufferView::get<T>(index);
        }

        reference operator[] (size_type index) {
            return edit(index);
        }
        const_reference operator[] (size_type index) const {
            return get(index);
        }

        value_type* data() {
            return &edit(0);
        }
        const value_type* data() const {
            return &get(0);
        }

        // std::vector Modifiers:
        void clear() {
            _numElements = 0;
        }

        // assign calls force the buffer to the specified size
        void assign(size_type numElements, const value_type& element) {
            resize(numElements);
            for (size_type i = 0; i < numElements; i++) {
                edit[i] = element;
            }
        }

        void assign(size_type numElements, const value_type* elements) {
            resize(numElements);
            setSubData(0, numElements, elements);
        }

        void assign(const std::vector<value_type>& elements) {
            assign(elements.size(), elements.data());
        }

        // assign range only resize the buffer if offset + numElements is over the current buffer size
        void assignRange(size_type offset, size_type numElements, const value_type* elements) {
            auto totalNewSize = offset + numElements;
            if (totalNewSize > size()) {
                resize(totalNewSize);
            }
            setSubData(offset, numElements, elements);
        }

        void assignRange(size_type offset, const std::vector<value_type>& elements) {
            assign(offset, elements.size(), elements.data());
        }

    protected:
        Index _numElements { 0 };
    };


    template <class T> class AvatarElementBuffer : public ElementBuffer<T> {
    public:
        using Parent = ElementBuffer<T>;

        AvatarElementBuffer() : Parent() {}
        AvatarElementBuffer(size_type capacity) : Parent(capacity) {}

    };

    template <class T> class JointElementBuffer : public ElementBuffer<T> {
    public:
        using Parent = ElementBuffer<T>;

        JointElementBuffer() : Parent() {}
        JointElementBuffer(size_type capacity) : Parent(capacity) {}

    };
}

#endif // hifi_crowd_Flock_h
