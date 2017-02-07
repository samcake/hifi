#include "Image.h"


int image::BC::cpp { 0 };

namespace image {

    namespace pixel {
        template <> const RGB16_565 mix(const RGB16_565 p0, const RGB16_565 p1, const Byte alpha) {
            return RGB16_565(
                mix5_4(p0.r, p1.r, alpha),
                mix6_4(p0.g, p1.g, alpha),
                mix5_4(p0.b, p1.b, alpha));
        }
    
        template <> const RGB32 mix(const RGB32 p0, const RGB32 p1, const float alpha) {
            Byte a = 255 * alpha;
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


        template <> RGB32 filterQuadBox(const RGB32& p00, const RGB32& p10, const RGB32& p01, const RGB32& p11) {
            return RGB32((p00.r + p10.r + p11.r + p01.r) * (127 * 127) / (255 * 255),
            //    ((int)p00.g + (int)p10.g + (int)p11.g + (int)p01.g) * (127 * 127) / (255 * 255),
                0,
                ((int)p00.b + (int)p10.b + (int)p11.b + (int)p01.b) * (127 * 127) / (255 * 255));
        }
        template <> RGBA32 filterQuadBox(const RGBA32& p00, const RGBA32& p10, const RGBA32& p01, const RGBA32& p11) {
            return RGBA32(((int)p00.r + (int)p10.r + (int)p11.r + (int)p01.r) * (127 * 127) / (255 * 255),
                    ((int)p00.g + (int)p10.g + (int)p11.g + (int)p01.g) * (127 * 127) / (255 * 255),
                    ((int)p00.b + (int)p10.b + (int)p11.b + (int)p01.b) * (127 * 127) / (255 * 255),
                    ((int)p00.a + (int)p10.a + (int)p11.a + (int)p01.a) * (127 * 127) / (255 * 255));
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
