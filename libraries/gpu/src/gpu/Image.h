//
//  Image.h
//  libraries/gpu/src/gpu
//
//  Created by Sam Gateau on 1/17/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#ifndef hifi_gpu_Image_h
#define hifi_gpu_Image_h

#include <glm/glm.hpp>
#include "Forward.h"


namespace image {

    // Storage types
    using Byte = uint8_t;
    using Byte2 = uint16_t;
    using Byte4 = uint32_t;
    using Byte8 = uint64_t;
    
    // Storage type selector based on size (sizeof of a type)
    template<int size, typename T1 = Byte, typename T2 = Byte2, typename T4 = Byte4, typename T8 = Byte8>
    struct storage { typedef T1 type; };
    
    template<typename T1, typename T2, typename T4, typename T8>
    struct storage<2, T1, T2, T4, T8> { typedef T2 type; };
    
    template<typename T1, typename T2, typename T4, typename T8>
    struct storage<4, T1, T2, T4, T8> { typedef T4 type; };
    
    template<typename T1, typename T2, typename T4, typename T8>
    struct storage<8, T1, T2, T4, T8> { typedef T8 type; };
    
    static const Byte BLACK8 { 0 };
    static const Byte WHITE8 { 255 };
    
    template <int N> int bitVal() { return 1 << N; }
    template <int Tn, int An> int bitProduct() { return bitVal<Tn>() * bitVal<An>(); }
    template <int Tn, int An, typename T = Byte, typename A = T> T mix(const T x, const T y, const A a) { return T(((bitVal<An>() - a) * x + a * y) / bitProduct<Tn, An>()); }

    Byte mix5_4(const Byte x, const Byte y, const Byte a);// { return mix<5, 4>(x, y, a); }
    Byte mix6_4(const Byte x, const Byte y, const Byte a);// { return mix<6, 4>(x, y, a); }
    Byte mix8_8(const Byte x, const Byte y, const Byte a);// { return mix<8, 8>(x, y, a); }

    // Coordinate and count types
    using Coord = uint16_t;
    using Coord2 = glm::u16vec2;
    using Count = uint32_t;

    // Maximum pixel along one direction coord is 32768 
    static const Coord MAX_COORD { 32768 };
    // Maximum number of pixels in an image
    static const Count MAX_COUNT { MAX_COORD * MAX_COORD };
    static const Count MAX_INDEX { MAX_COUNT };

    struct MemBlock {
        size_t _rowSize {0};
        size_t _byteSize {0};
        Byte* _bytes {nullptr};

        Byte* data() {
            return _bytes;
        }
        const Byte* data() const {
            return _bytes;
        }

        size_t byteSize() const { return _byteSize; }
        size_t rowSize() const { return _rowSize; }

        ~MemBlock() { if (_bytes) { delete _bytes; } }
        MemBlock() {}
        MemBlock(size_t rowSize, size_t byteSize) :
                _rowSize(rowSize),
                _byteSize(byteSize)
        {
            if (_byteSize) { _bytes = new Byte[_byteSize]; }
        }
    };

    namespace pixel {
        
        struct RGB32 {
            Byte r { BLACK8 };
            Byte g { BLACK8 };
            Byte b { BLACK8 };
            Byte x { WHITE8 };

            RGB32() {}
            RGB32(const RGB32& rgb) : r(rgb.r), g(rgb.g), b(rgb.b) {}
            RGB32(Byte pR, Byte pG, Byte pB) : r(pR), g(pG), b(pB) {}
        };

        struct SRGB32 {
            Byte r { BLACK8 };
            Byte g { BLACK8 };
            Byte b { BLACK8 };
            Byte x { WHITE8 };

            SRGB32() {}
            SRGB32(const SRGB32& rgb) : r(rgb.r), g(rgb.g), b(rgb.b) {}
            SRGB32(Byte pR, Byte pG, Byte pB) : r(pR), g(pG), b(pB) {}
        };
        
        struct RGBA32 {
            Byte r { BLACK8 };
            Byte g { BLACK8 };
            Byte b { BLACK8 };
            Byte a { WHITE8 };

            RGBA32() {}
            RGBA32(const RGBA32& rgba) : r(rgba.r), g(rgba.g), b(rgba.b), a(rgba.a) {}
            RGBA32(Byte pR, Byte pG, Byte pB, Byte pA) : r(pR), g(pG), b(pB), a(pA) {}
        };

        struct R10G10B12 {
            Byte4 r : 10;
            Byte4 g : 10;
            Byte4 b : 12;
        };
        
        struct RGB16_565 {
            Byte2 b : 5;
            Byte2 g : 6;
            Byte2 r : 5;
            
            RGB16_565() : b(BLACK8), g(BLACK8), r(BLACK8) {}
            RGB16_565(Byte pR, Byte pG, Byte pB) : b(pB), g(pG), r(pR) {}
        };
        
        struct R8 {
            Byte r { BLACK8 };
        };
        

        template <typename F, typename S> const F mix(const F p0, const F p1, const S alpha) { return p0; }
        template <> const RGB32 mix(const RGB32 p0, const RGB32 p1, const float alpha);
        template <> const RGB32 mix(const RGB32 p0, const RGB32 p1, const Byte alpha);
        template <> const RGB16_565 mix(const RGB16_565 p0, const RGB16_565 p1, const Byte alpha);

        template <typename F> F filterQuadBox(const F quad[4]) { return quad[0]; }
        template <> RGB32 filterQuadBox(const RGB32 quad[4]);
        template <> RGBA32 filterQuadBox(const RGBA32 quad[4]);
        template <> SRGB32 filterQuadBox(const SRGB32 quad[4]);


        template <typename F, typename S = typename storage<sizeof(F)>::type > class Pixel {


        public:
            using This = Pixel<F,S>;
            using Format = F;
            using Storage = S;

            static const uint32_t SIZE { sizeof(S) };

            static void copy(F* dest, const F* src) { memcpy(dest, src, SIZE); }

            Format val { Format() };

            Pixel() {}
            Pixel(const Format& v) : val(v) {}
            Pixel(const Storage& s) : val(storageToFormatConst(s)) {}

            Pixel& operator = (const Pixel& src) {
                *editStorage() = *src.storage();
                return (*this);
            }

            const Storage* storage() const { return reinterpret_cast<const Storage*> (this); }
            Storage* editStorage() { return reinterpret_cast<Storage*> (this); }

            static const Format& storageToFormatConst(const Storage& s) { return *(reinterpret_cast<const This*>(&s)); }
            static Format& storageToFormat(Storage& s) { return *(reinterpret_cast<This*>(&s)); }

            static This* cast(Storage* s) { return reinterpret_cast<This*>(s); }
            static const This* cast(const Storage* s) { return reinterpret_cast<const This*>(s); }
        };

        template <typename P, int length> class PixelBlock {
        public:
            using Format = typename P::Format;
            using Storage = typename P::Storage;

            static const uint16_t LENGTH { length };
            static const uint32_t SIZE { length * sizeof(P) };

            P pixels[length];

            PixelBlock() {}

            PixelBlock(const P* srcPixels) {
                setPixels(srcPixels);
            }

            void setPixels(const P* srcPixels) {
                memcpy(pixels, srcPixels, SIZE);
            }

            const Storage* storage() const { return pixels->storage(); }
        };
        
        template <typename P, int tileW, int tileH> class Tile {
        public:
            using Format = typename P::Format;
            using Storage = typename P::Storage;

            using Block = PixelBlock<P, tileW * tileH>;

            uint16_t getWidth() const { return tileW; }
            uint16_t getHeight() const { return tileH; }

            Block _block;

            Tile() {}
            Tile(const P* srcPixels) : _block(srcPixels) {}
            
            
        };
    };

    using PixRGB565 = pixel::Pixel<pixel::RGB16_565>;
    using PixRGB32 = pixel::Pixel<pixel::RGB32>;
    using PixRGBA32 = pixel::Pixel<pixel::RGBA32>;
    using PixSRGB32 = pixel::Pixel<pixel::SRGB32>;
    using PixR10G10B12 = pixel::Pixel<pixel::R10G10B12>;
    using PixR8 = pixel::Pixel<pixel::R8>;

    class BC {
    public:
        static int cpp;
        
        struct BC1 {
            PixRGB565 color0;
            PixRGB565 color1;
            Byte4 table;
        };
        
        struct BC4 {
            PixRGB565 color0;
            PixRGB565 color1;
            Byte4 table;
        };
    };
    
    template <typename F, typename S = typename storage<sizeof(F)>::type> class CompressedBlock {
    public:
        using Format = F;
        using Storage = S;

        static const uint32_t SIZE { sizeof(F) };

        Format bc;

        CompressedBlock() {}

        const Storage* storage() const { return static_cast<Storage> (&bc); }
    };
    
    
    template <typename PB, typename CB> void compress(const PB& srcBlock, CB& dstBlock) { }
    template <typename PB, typename CB> void uncompress(const CB& srcBlock, PB& dstBlock) { }


    using PB_RGB32 = pixel::PixelBlock<PixRGB32, 16>;
    using PB_RGBA32 = pixel::PixelBlock<PixRGBA32, 16>;
    
    using CB_BC1 = CompressedBlock<BC::BC1>;
    using CB_BC4 = CompressedBlock<BC::BC4>;
    
    template <> void compress(const PB_RGB32& src, CB_BC1& dst);
    template <> void compress(const PB_RGBA32& src, CB_BC4& dst);

    template <> void uncompress(const CB_BC1& src, PB_RGB32& dst);
    template <> void uncompress(const CB_BC4& src, PB_RGBA32& dst);
    
    
    template <typename P, int rowAlignement = 4>
    class PixelArray {
    public:
        using This = PixelArray<P>;
        using Pixel = P;
        using Storage = typename P::Storage;

        static uint32_t evalPadding(size_t rowSize) {
            const int rowAlignementMinusOne = rowAlignement - 1;
            return (uint32_t) (rowAlignementMinusOne - (rowSize + rowAlignementMinusOne) % rowAlignement);
        }
        
        static size_t evalRowByteSize(int numPixels) {
            size_t netSize = numPixels * Pixel::SIZE;
            return netSize + evalPadding(netSize);
        };
        
        static Coord evalNumPixelsPerRow(size_t rowSize) {
            size_t rowNumPixels = rowSize / Pixel::SIZE;
         //   if (byteSize > rowNumPixels * Pixel::SIZE) {
         //       rowNumPixels++;
         //   }
            return (Coord)rowNumPixels;
        };
        static size_t evalByteSize(Coord width, Coord height) {
            auto rowSize = evalRowByteSize(width);
            return rowSize * height;
        };
        static Count evalNumPixels(size_t bytesSize, size_t rowSize) {
            auto width = (Count) evalNumPixelsPerRow(rowSize);
            auto height = (Count) (bytesSize / rowSize);
            return width * height;
        };

        PixelArray() {
            resetBytes(0, 0, nullptr);
        }
        PixelArray(size_t rowSize, size_t byteSize, const void* bytes) {
            resetBytes(rowSize, byteSize, bytes);
        }
        PixelArray(const PixelArray& src) {
            resetBytes(src.rowSize(), src.byteSize(), src.readBytes(0));
        }
        PixelArray(PixelArray&& src) {
            _buffer = std::move(src._buffer);
        }
        This& operator = (const This& src) {
            resetBytes(src.rowSize(), src.byteSize(), src.readBytes(0));
            return (*this);
        }
        This& operator = (This&& src) {
            _buffer = std::move(src._buffer);
            return (*this);
        }
    
        size_t byteSize() const { return _buffer->byteSize(); }
        size_t rowSize() const { return _buffer->rowSize(); }

        Count numRows() const { return _buffer->byteSize() / _buffer->rowSize(); }
        Count numPixelsPerRow() const { return evalNumPixelsPerRow(_buffer->rowSize()); }
        Count numPixels() const { return numRows() * numPixelsPerRow(); }

        //access storage or pixel types at row index in the buffer
        const Storage* readBytes(const Coord row) const { return reinterpret_cast<const Storage*>(_buffer->data() + rowSize() * row); }
        const Pixel* readPixels(const Coord row) const { return Pixel::cast(readBytes(row)); }
        
        Storage* editBytes(const Coord row) { return reinterpret_cast<Storage*>(_buffer->data() + rowSize() * row); }
        Pixel* editPixels(const Coord row) { return Pixel::cast(editBytes(row)); }
    
    private:
        std::unique_ptr<MemBlock> _buffer;

        void resetBytes(size_t rowSize, size_t byteSize, const void* bytes) {
            _buffer.reset(new MemBlock(rowSize, byteSize));
            if (byteSize && bytes) {
                memcpy(_buffer->data(), bytes, byteSize);
            }
        }
    };

    template <typename B>
    class PixelBlockArray {
    public:
        using Block = B;
        using Blocks = std::vector<Block>;

        static int evalNumBlocks(size_t byteSize) {
            size_t numBlocks = byteSize / Block::SIZE;
            if (byteSize > numBlocks * Block::SIZE) {
                numBlocks++;
            }
            return (int) numBlocks;
        };
        static size_t evalByteSize(int numBlocks) {
            return numBlocks * Block::SIZE;
        };

        PixelBlockArray(size_t byteSize, const void* bytes) {
            setBytes(byteSize, bytes);
        }

        int numBlocks() const { return evalByteSize(_blocks.size()); }
        size_t byteSize() const { return evalByteSize(_blocks.size()); }

        Blocks _blocks;

    private:
        void setBytes(size_t byteSize, const void* bytes) {
            _blocks = Blocks(evalNumBlocks(byteSize));
            if (bytes) {
                memcpy(_blocks.data(), bytes, byteSize);
            }
        }
    };
    
    class Grid {
    public:
        using Coord = uint16_t;
        using Coord2 = glm::u16vec2;
        using Size = uint32_t;
        
        static const Coord2 TILE_PIXEL;
        static const Coord2 TILE_QUAD;
        static const Coord2 TILE_DEFAULT;
        
        Grid(const Coord2& surface, const Coord2& tile = TILE_DEFAULT) : _surface(surface), _tile(tile) {}
        Grid(Coord width, Coord height, const Coord2& tile = TILE_DEFAULT) : _surface(width, height), _tile(tile) {}
        
        Coord2 _surface { 1, 1 };
        Coord2 _tile { TILE_DEFAULT };
        
        Coord width() const { return _surface.x; }
        Coord height() const { return _surface.y; }
        const Coord2& size() const { return _surface; }

        Coord tileWidth() const { return evalNumTiles(_surface.x, _tile.x); }
        Coord tileHeight() const { return evalNumTiles(_surface.y, _tile.y); }
        Coord2 tileSize() const { return Coord2(tileWidth(), tileHeight()); }
    
    
        Coord2 toTile(const Coord2& pixel) const { return pixel / _tile; }
        Coord2 toTileSubpix(const Coord2& pixel) const { return pixel % _tile; }
        Coord2 toTile(const Coord2& pixel, Coord2& subpix) const {
            subpix = toTileSubpix(pixel);
            return toTile(pixel);
        }
        
        Coord2 toPixel(const Coord2& tile) const { return tile * _tile; }
        Coord2 toPixel(const Coord2& tile, const Coord2& subpix) const { return tile * _tile + subpix; }
        
        
        static Coord evalNumTiles(Coord pixelLength, Coord tileLength) {
            auto tilePos = pixelLength / tileLength;
            if (tilePos * tileLength < pixelLength) {
                tilePos++;
            }
            return tilePos;
        }
    };
    
    template <typename T>
    class Tilemap {
    public:
        using Tile = T;
        using Block = typename T::Block;

        Grid _grid;
        PixelBlockArray<Block> _blocks;
        
        void resize(const Grid::Coord2& widthHeight) {
            _grid = Grid(widthHeight, Coord2(Tile::getWidth(), Tile::getHeight()));
            
        }
       
    };

    class Dim {
    public:

        Coord2 _dims { 0 };

        static Coord cap(Coord c) { return (c < MAX_COORD ? c : MAX_COORD); }
        static Coord2 cap(const Coord2& dims) { return Coord2(cap(dims.x), cap(dims.y)); }

        static Count numPixels(const Coord2& dims) { return Count(cap(dims.x)) * Count(cap(dims.y)); }


        static Coord nextMip(Coord c) { return (c > 1 ? (c >> 1) : c); }
        static Coord2 nextMip(const Coord2& dims) { return Coord2(nextMip(dims.x), nextMip(dims.y)); }

        Dim(Coord w, Coord h) : _dims(w, h) {}
        Dim(const Coord2& dims) : _dims(dims) {}

        Count numPixels() const { return Dim::numPixels(_dims); }
        Dim nextMip() const { return Dim(nextMip(_dims)); }

        int maxLevel() const {
            int level = 0;
            auto dim = (*this);
            while (dim._dims.x > 1 || dim._dims.y > 1) {
                level++;
                dim = dim.nextMip();
            }
            return level;
        }
    };

    template < typename P > class Surface {
    public:
        using This = Surface<P>;
        using Pixel = P;
        using Format = typename P::Format;
        using Pixels = PixelArray<P>;

        Dim _dim { 0, 0 };
        Pixels _pixels;

        using PixelPair = Pixel[2];

        Surface()
        {}
        Surface(const Surface& src) :
            _dim(src._dim),
            _pixels(src._pixels) 
        {}
        Surface(Surface&& src) :
            _dim(src._dim),
            _pixels(std::move(src._pixels))
        {}

        Surface& operator = (const Surface& src) {
            _dim = (src._dim);
            _pixels = (src._pixels);
            return (*this);
        }

        Surface& operator = (Surface&& src) {
            _dim = (src._dim);
            _pixels = std::move(src._pixels);
            return (*this);
        }

        Surface(Coord width, Coord height, size_t byteSize = 0, const void* bytes = nullptr) :
            _dim(width, height)
        {
            auto rowSize = Pixels::evalRowByteSize(width);
            auto pixelsSize =  Pixels::evalByteSize(width, height);
            if (byteSize >= pixelsSize && bytes) {
                _pixels = Pixels(rowSize, pixelsSize, bytes);
            } else {
                _pixels = Pixels(rowSize, pixelsSize, nullptr);
            }
        }

        This nextMip() const {
            Dim subDim = _dim.nextMip();

            auto sub = Surface<P>(subDim._dims.x, subDim._dims.y);
            {
                for (int y = 0; y < subDim._dims.y; y++) {
                    // get pixels from source at 2x, 2x +1 2y, 2y +1
                    auto srcLine0 = (_pixels.readPixels(2 * y));
                    auto srcLine1 = (_pixels.readPixels(2 * y + 1));

                    // Dest
                    auto destLine = sub._pixels.editPixels(y);

                    for (int x = 0; x < subDim._dims.x; x++) {

                        // filter and assign to dest
                        Format srcQuad[4] = { (*srcLine0).val, (*(srcLine0 + 1)).val, (*srcLine1).val, (*(srcLine1 + 1)).val };
                        (*destLine) = pixel::filterQuadBox<Format>(srcQuad);

                        // next
                        destLine++;
                        srcLine0 += 2;
                        srcLine1 += 2;
                    }
                }
            }
            return sub;
        }
        
        void downsample(std::vector<This>& mips, int num) const {
            if (num == 0) {
                return;
            }
            auto maxLevel = _dim.maxLevel();
        
            if (num == -1 || num > maxLevel) {
                num = maxLevel;
            }
            else if (num < -1) {
                return;
            }
            mips.resize(num);
            
            mips[0] = std::move(nextMip());
        
            for (int i = 1; i < num; i++) {
                mips[i] = (std::move(mips[i-1].nextMip()));
            }
        }
    };


}

#endif
