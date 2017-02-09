//
//  Image.cpp
//  libraries/gpu/src/gpu
//
//  Created by Sam Gateau on 1/17/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "Image.h"


int image::BC::cpp { 0 };
extern const float srgbToLinearLookupTable[256];

float sRGB8ToLinearFloat(const uint8_t srgb) {
    return srgbToLinearLookupTable[srgb];
}

uint8_t linearFloatTosRGB8(const float l) {
 /*   static bool set = false;
    static uint8_t linearTorgbLookupTable[256];
    if (!set) {
        linearTorgbLookupTable[0] = 0.0f;
        for (int i = 1; i < 255; i++) {
            const float SRGB_ELBOW_INV = 0.0031308f;
            float linear = i / 255.0f;
            // This should mirror the conversion table found in section 17.3.9: sRGB Conversion
            if (linear < SRGB_ELBOW_INV) {
                linearTorgbLookupTable[i] = (uint8_t) (255.f * 12.92f * linear);
            } else if (SRGB_ELBOW_INV <= linear) {
                linearTorgbLookupTable[i] = (uint8_t) (255.f * 1.055f * powf(linear, 0.41666f - 0.055f));
            }
        }
        linearTorgbLookupTable[255] = 1.0f;
        set = true;
    }
    return linearTorgbLookupTable[(uint8_t) (l * 255.0)];*/

     const float SRGB_ELBOW_INV = 0.0031308f;
            float linear = l;
            // This should mirror the conversion table found in section 17.3.9: sRGB Conversion
            if (linear <= 0.0f) {
                return 0;
            }
            if (linear >= 1.0f) {
                return 255;
            }

            if (linear < SRGB_ELBOW_INV) {
                return (uint8_t) (255.f * 12.92f * linear);
            } else {
                return (uint8_t) (255.f * 1.055f * powf(linear, 0.41666f - 0.055f));
            }
}

namespace image {
    Byte mix5_4(const Byte x, const Byte y, const Byte a) { return mix<5, 4>(x, y, a); }
    Byte mix6_4(const Byte x, const Byte y, const Byte a) { return mix<6, 4>(x, y, a); }
    Byte mix8_8(const Byte x, const Byte y, const Byte a) { return mix<8, 8>(x, y, a); }

    namespace pixel {
        template <> const RGB16_565 mix(const RGB16_565 p0, const RGB16_565 p1, const Byte alpha) {
            return RGB16_565(
                mix5_4(p0.r, p1.r, alpha),
                mix6_4(p0.g, p1.g, alpha),
                mix5_4(p0.b, p1.b, alpha));
        }
    
        template <> const RGB32 mix(const RGB32 p0, const RGB32 p1, const float alpha) {
            Byte a = (Byte) (255 * alpha);
            return RGB32(
                mix8_8(p0.r, p1.r, a),
                mix8_8(p0.g, p1.g, a),
                mix8_8(p0.b, p1.b, a));
        }

        template <> const RGB32 mix(const RGB32 p0, const RGB32 p1, const Byte alpha) {
            return RGB32(
                mix8_8(p0.r, p1.r, alpha),
                mix8_8(p0.g, p1.g, alpha),
                mix8_8(p0.b, p1.b, alpha));
        }


        template <> RGB32 filterQuadBox(const RGB32 p[4]) {
            return RGB32(
                ((int)p[0].r + (int)p[1].r + (int)p[2].r + (int)p[3].r) * (127 * 127) / (255 * 255),
                ((int)p[0].g + (int)p[1].g + (int)p[2].g + (int)p[3].g) * (127 * 127) / (255 * 255),
                ((int)p[0].b + (int)p[1].b + (int)p[2].b + (int)p[3].b) * (127 * 127) / (255 * 255));
        }

        template <> SRGB32 filterQuadBox(const SRGB32 p[4]) {
            return SRGB32(
                linearFloatTosRGB8((sRGB8ToLinearFloat(p[0].r) + sRGB8ToLinearFloat(p[1].r) + sRGB8ToLinearFloat(p[2].r) + sRGB8ToLinearFloat(p[3].r)) * 0.25f),
                linearFloatTosRGB8((sRGB8ToLinearFloat(p[0].g) + sRGB8ToLinearFloat(p[1].g) + sRGB8ToLinearFloat(p[2].g) + sRGB8ToLinearFloat(p[3].g)) * 0.25f),
                linearFloatTosRGB8((sRGB8ToLinearFloat(p[0].b) + sRGB8ToLinearFloat(p[1].b) + sRGB8ToLinearFloat(p[2].b) + sRGB8ToLinearFloat(p[3].b)) * 0.25f) );
        }

        template <> RGBA32 filterQuadBox(const RGBA32 p[4]) {
            return RGBA32(
                    ((int)p[0].r + (int)p[1].r + (int)p[2].r + (int)p[3].r) * (127 * 127) / (255 * 255),
                    ((int)p[0].g + (int)p[1].g + (int)p[2].g + (int)p[3].g) * (127 * 127) / (255 * 255),
                    ((int)p[0].b + (int)p[1].b + (int)p[2].b + (int)p[3].b) * (127 * 127) / (255 * 255),
                    ((int)p[0].a + (int)p[1].a + (int)p[2].a + (int)p[3].a) * (127 * 127) / (255 * 255));

        }
    }


template <> void compress(const PB_RGB32& src, CB_BC1& dst) {
}

template <> void uncompress(const CB_BC1& src, PB_RGB32& dst) {
    auto bc1 = src.bc;
    
    auto c0 = bc1.color0.val;
    auto c1 = bc1.color1.val;
    
    for (int i = 0; i < PB_RGB32::LENGTH; ++i) {
        //dst.pixels[i] = ;
        auto r = pixel::mix(
                       c0,
                       c1,
                       (Byte)bc1.table);
    }
}

template <> void compress(const PB_RGBA32& src, CB_BC4& dst) {
    
}

template <> void uncompress(const CB_BC4& src, PB_RGBA32& dst) {

}

}
