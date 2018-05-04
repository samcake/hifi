//
//  Stream.h
//  interface/src/gpu
//
//  Created by Sam Gateau on 10/29/2014.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#ifndef hifi_gpu_Stream_h
#define hifi_gpu_Stream_h

#include <vector>
#include <map>
#include <array>
#include <string>
#include <bitset>

#include <assert.h>

#include "Resource.h"
#include "Format.h"

namespace gpu {

class Element;

// Stream namespace class
class Stream {
public:

    // Possible input slots identifiers
    enum InputSlot {
        POSITION = 0,
        NORMAL = 1,
        COLOR = 2,
        TEXCOORD0 = 3,
        TEXCOORD = TEXCOORD0,
        TANGENT = 4,
        SKIN_CLUSTER_INDEX = 5,
        SKIN_CLUSTER_WEIGHT = 6,
        TEXCOORD1 = 7,
        TEXCOORD2 = 8,
        TEXCOORD3 = 9,
        TEXCOORD4 = 10,

        NUM_INPUT_SLOTS,

        DRAW_CALL_INFO = 15, // Reserve last input slot for draw call infos
    };

    typedef uint8 Slot;

    static const std::array<Element, InputSlot::NUM_INPUT_SLOTS>& getDefaultElements();

    // Frequency describer
    enum Frequency {
        PER_VERTEX = 0,
        PER_INSTANCE = 1,
    };

    // The attribute description
    // Every thing that is needed to detail a stream attribute and how to interpret it
    class Attribute {
    public:
        Attribute() {}

        Attribute(Slot slot, Slot channel, Element element, Offset offset = 0, Frequency frequency = PER_VERTEX) :
            _slot(slot),
            _channel(channel),
            _element(element),
            _offset(offset),
            _frequency(frequency)
        {}

        Slot _slot{ POSITION }; // Logical slot assigned to the attribute
        Slot _channel{ POSITION }; // index of the channel where to get the data from
        Element _element{ Element::VEC3F_XYZ };
        Offset _offset{ 0 };
        uint32 _frequency{ PER_VERTEX };

        // Size of the 
        uint32 getSize() const { return _element.getSize(); }

        // Generate a string key describing the attribute uniquely
        std::string getKey() const;
    };

    // Stream Key is a trait description of a stream in terms of the available attributes
    class Key {
    public:
       enum FlagBit {
            POSITION_BIT = 0,
            NORMAL_BIT,
            TANGENT_BIT,
            COLOR_BIT,
            TEXCOORD0_BIT,
            TEXCOORD1_BIT,
            SKIN_BIT,   // Has the skin weights and indices
            SMOOTH_NORMAL_BIT,

            NUM_FLAGS,
        };
        typedef std::bitset<NUM_FLAGS> Flags;

        // The signature is the Flags
        Flags _flags;

        Key() : _flags(0) {}
        Key(const Flags& flags) : _flags(flags) {}

        class Builder {
            Flags _flags{ 0 };
        public:
            Builder() {}

            Key build() const { return Key(_flags); }

            Builder& withPosition() { _flags.set(POSITION_BIT); return (*this); }
            Builder& withNormal() { _flags.set(NORMAL_BIT); return (*this); }
            Builder& withTangent() { _flags.set(TANGENT_BIT); return (*this); }

            Builder& withColor() { _flags.set(COLOR_BIT); return (*this); }
            Builder& withTexcoord0() { _flags.set(TEXCOORD0_BIT); return (*this); }
            Builder& withTexcoord1() { _flags.set(TEXCOORD1_BIT); return (*this); }
            Builder& withTexcoords() { return withTexcoord0().withTexcoord1(); }

            Builder& withSkin() { _flags.set(SKIN_BIT); return (*this); }

            Builder& withSmoothNormal() { _flags.set(SMOOTH_NORMAL_BIT); return (*this); }

            // Convenient standard keys that we will keep on using all over the place
            static Key pos() { return Builder().withPosition().build(); }
        };

        void setPosition(bool value) { _flags.set(POSITION_BIT, value); }
        bool isPosition() const { return _flags[POSITION_BIT]; }

        void setNormal(bool value) { _flags.set(NORMAL_BIT, value); }
        bool isNormal() const { return _flags[NORMAL_BIT]; }

        void setTangent(bool value) { _flags.set(TANGENT_BIT, value); }
        bool isTangent() const { return _flags[TANGENT_BIT]; }

        void setColor(bool value) { _flags.set(COLOR_BIT, value); }
        bool isColor() const { return _flags[COLOR_BIT]; }

        void setTexcoord0(bool value) { _flags.set(TEXCOORD0_BIT, value); }
        bool isTexcoord0() const { return _flags[TEXCOORD0_BIT]; }
        void setTexcoord1(bool value) { _flags.set(TEXCOORD1_BIT, value); }
        bool isTexcoord1() const { return _flags[TEXCOORD1_BIT]; }
        bool isTexcoords() const { return isTexcoord1() || isTexcoord0(); }

        void setSkin(bool value) { _flags.set(SKIN_BIT, value); }
        bool isSkin() const { return _flags[SKIN_BIT]; }

        void setSmoothNormal(bool value) { _flags.set(SMOOTH_NORMAL_BIT, value); }
        bool isSmoothNormal() const { return _flags[SMOOTH_NORMAL_BIT]; }

    };


    class Filter {
    public:
        Key::Flags _value{ 0 };
        Key::Flags _mask{ 0 };

        Filter(const Key::Flags& value = Key::Flags(0), const Key::Flags& mask = Key::Flags(0)) : _value(value), _mask(mask) {}

        class Builder {
            Key::Flags _value{ 0 };
            Key::Flags _mask{ 0 };
        public:
            Builder() {}

            Filter build() const { return Filter(_value, _mask); }

            Builder& withoutPosition()       { _value.reset(Key::POSITION_BIT); _mask.set(Key::POSITION_BIT); return (*this); }
            Builder& withPosition()        { _value.set(Key::POSITION_BIT);  _mask.set(Key::POSITION_BIT); return (*this); }

            Builder& withoutNormal()       { _value.reset(Key::NORMAL_BIT); _mask.set(Key::NORMAL_BIT); return (*this); }
            Builder& withNormal()        { _value.set(Key::NORMAL_BIT);  _mask.set(Key::NORMAL_BIT); return (*this); }

            Builder& withoutTangent()       { _value.reset(Key::TANGENT_BIT); _mask.set(Key::TANGENT_BIT); return (*this); }
            Builder& withTangent()        { _value.set(Key::TANGENT_BIT);  _mask.set(Key::TANGENT_BIT); return (*this); }

            Builder& withoutColor()       { _value.reset(Key::COLOR_BIT); _mask.set(Key::COLOR_BIT); return (*this); }
            Builder& withColor()        { _value.set(Key::COLOR_BIT);  _mask.set(Key::COLOR_BIT); return (*this); }

            Builder& withoutTexcoord0()       { _value.reset(Key::TEXCOORD0_BIT); _mask.set(Key::TEXCOORD0_BIT); return (*this); }
            Builder& withTexcoord0()        { _value.set(Key::TEXCOORD0_BIT);  _mask.set(Key::TEXCOORD0_BIT); return (*this); }

            Builder& withoutTexcoord1()       { _value.reset(Key::TEXCOORD1_BIT); _mask.set(Key::TEXCOORD1_BIT); return (*this); }
            Builder& withTexcoord1()        { _value.set(Key::TEXCOORD1_BIT);  _mask.set(Key::TEXCOORD1_BIT); return (*this); }

            Builder& withoutSkin()       { _value.reset(Key::SKIN_BIT); _mask.set(Key::SKIN_BIT); return (*this); }
            Builder& withSkin()        { _value.set(Key::SKIN_BIT);  _mask.set(Key::SKIN_BIT); return (*this); }

            Builder& withoutSmoothNormal()       { _value.reset(Key::SMOOTH_NORMAL_BIT); _mask.set(Key::SMOOTH_NORMAL_BIT); return (*this); }
            Builder& withSmoothNormal()        { _value.set(Key::SMOOTH_NORMAL_BIT);  _mask.set(Key::SMOOTH_NORMAL_BIT); return (*this); }

            // Convenient standard keys that we will keep on using all over the place
            static Filter position() { return Builder().withPosition().build(); }
        };

        // Item Filter operator testing if a key pass the filter
        bool test(const Key& key) const { return (key._flags & _mask) == (_value & _mask); }

        class Less {
        public:
            bool operator() (const Filter& left, const Filter& right) const {
                if (left._value.to_ulong() == right._value.to_ulong()) {
                    return left._mask.to_ulong() < right._mask.to_ulong();
                } else {
                    return left._value.to_ulong() < right._value.to_ulong();
                }
            }
        };
    };

    // Stream Format is describing how to feed a list of attributes from a bunch of stream buffer channels
    class Format {
    public:
        typedef std::map< Slot, Attribute > AttributeMap;

        class ChannelInfo {
        public:
            std::vector< Slot > _slots;
            std::vector< Offset > _offsets;
            Offset _stride;
            uint32 _netSize;

            ChannelInfo() : _stride(0), _netSize(0) {}
        };
        typedef std::map< Slot, ChannelInfo > ChannelMap;

        size_t getNumAttributes() const { return _attributes.size(); }
        const AttributeMap& getAttributes() const { return _attributes; }

        size_t getNumChannels() const { return _channels.size(); }
        const ChannelMap& getChannels() const { return _channels; }
        Offset getChannelStride(Slot channel) const { return _channels.at(channel)._stride; }

        size_t getElementTotalSize() const { return _elementTotalSize; }

        bool setAttribute(Slot slot, Slot channel, Element element, Offset offset = 0, Frequency frequency = PER_VERTEX);
        bool setAttribute(Slot slot, Frequency frequency = PER_VERTEX);
        bool setAttribute(Slot slot, Slot channel, Frequency frequency = PER_VERTEX);

        bool hasAttribute(Slot slot) const { return (_attributes.find(slot) != _attributes.end()); }
        Attribute getAttribute(Slot slot) const;

        const std::string& getKey() const { return _key; }
       const Key& getStreamKey() const { return _streamKey; }

        const GPUObjectPointer gpuObject{};

    protected:
        AttributeMap _attributes;
        ChannelMap _channels;
        uint32 _elementTotalSize { 0 };
        std::string _key;
        Key _streamKey;

        void evaluateCache();
    };

    typedef std::shared_ptr<Format> FormatPointer;
};

typedef std::vector< Offset > Offsets;

// Buffer Stream is a container of N Buffers and their respective Offsets and Srides representing N consecutive channels.
// A Buffer Stream can be assigned to the Batch to set several stream channels in one call
class BufferStream {
public:
    using Strides = Offsets;

    void clear() { _buffers.clear(); _offsets.clear(); _strides.clear(); }
    void addBuffer(const BufferPointer& buffer, Offset offset, Offset stride);

    const Buffers& getBuffers() const { return _buffers; }
    const Offsets& getOffsets() const { return _offsets; }
    const Strides& getStrides() const { return _strides; }
    size_t getNumBuffers() const { return _buffers.size(); }

    BufferStream makeRangedStream(uint32 offset, uint32 count = -1) const;

protected:
    Buffers _buffers;
    Offsets _offsets;
    Strides _strides;
};
typedef std::shared_ptr<BufferStream> BufferStreamPointer;

};


#endif
