//
//  Path.h
//  libraries/gpu/src/path
//
//  Created by Sam Gateau on 10/15/2015.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#ifndef hifi_gpu_Path_h
#define hifi_gpu_Path_h

#include <map>

#include "../gpu/Resource.h"

namespace gpu {
namespace pr {
// box2 - template class for 2-tuple vector
template <class T>
class box2 {
public:
    
    typedef T value_type;
    typedef gpu::Vec2 Vec2;
    typedef glm::mat3 Mat3;
    
    // Default/scalar constructor
    box2() {}
    box2(const Vec2 & p) { setMinMax( p, p ); }
    
    box2(const Vec2 & min, const Vec2& max) { setMinMax( min, max );}
    
    void setMinMax(const Vec2 & min, const Vec2& max) { cmin = min; cmax = max; }
    
    void evalCenterHalfsize( Vec2& center, Vec2& halfsize ) const {
        halfsize = T(0.5) * ( cmax - cmin );
        center = cmin + halfsize;
    }
    
    Vec2 center() const { return T(0.5) * ( cmax + cmin ); }
    Vec2 halfsize() const { return T(0.5) * ( cmax - cmin ); }
    
    Vec2 evalCorner( int index ) const {
        Vec2 size = cmax - cmin;
        return cmin + Vec2( size.x * ( index & 0x1 ), size.y * (( index & 0x2 ) >> 1) );
    }
    
    bool isNull() const {
        return  ( cmin.x == std::numeric_limits< T >::infinity() ) &&
        ( cmin.y == std::numeric_limits< T >::infinity() ) &&
        ( cmax.x == -std::numeric_limits< T >::infinity() ) &&
        ( cmax.y == -std::numeric_limits< T >::infinity() );
    }
    
    void setNull() { setMinMax( Vec2( std::numeric_limits<T>::infinity() ), Vec2( -std::numeric_limits<T>::infinity() ) ); }
    
    bool isInvalid() const {
        return  ( cmin.x == std::numeric_limits< T >::infinity() ) &&
        ( cmin.y == std::numeric_limits< T >::infinity() ) &&
        ( cmax.x == -std::numeric_limits< T >::infinity() ) &&
        ( cmax.y == -std::numeric_limits< T >::infinity() );
    }
    
    bool isFinite() const {
        return  ( cmin.x < std::numeric_limits< T >::infinity() ) ||
        ( cmin.y < std::numeric_limits< T >::infinity() ) ||
        ( cmax.x > -std::numeric_limits< T >::infinity() ) ||
        ( cmax.y > -std::numeric_limits< T >::infinity() );
    }
    
    // Merge a point to this box
    friend box2<T> & operator += ( box2<T> &lhs, const Vec2& rhs ) {
        lhs.cmin = min( lhs.cmin, rhs );
        lhs.cmax = max( lhs.cmax, rhs );
        return lhs;
    }
    
    // Merge another box with this box
    friend box2<T> & operator += ( box2<T> &lhs, const box2<T> & rhs ) {
        if ( rhs.isNull() )
            return lhs;
        
        lhs += rhs.cmin;
        lhs += rhs.cmax;
        return lhs;
    }
    
    // Transform the Box by the specified Matrix
    friend box2<T> operator * ( const Mat3 & lhs, const box2<T> & rhs) {
        if ( rhs.isNull() )
            return box2<T>();
        
        box2<T> result( Vec2( lhs * Vec3( rhs.cmin, T(1.0) ) ) );
        result += Vec2( lhs * Vec3( rhs.cmin.x, rhs.cmax.y, T(1.0) ) );
        result += Vec2( lhs * Vec3( rhs.cmax.x, rhs.cmin.y, T(1.0) ) );
        result += Vec2( lhs * Vec3( rhs.cmax, T(1.0) ) );
        
        return result;
    }
    

    
    static bool intersect( const box2<T> &lhs, const box2<T> &rhs ) {
        if ( lhs.isNull() )
            return false;
        if ( rhs.isNull() )
            return false;
        if ( lhs.cmin.x >= rhs.cmax.x )
            return false;
        if ( lhs.cmin.y >= rhs.cmax.y )
            return false;
        if ( rhs.cmin.x >= lhs.cmax.x )
            return false;
        if ( rhs.cmin.y >= lhs.cmax.y )
            return false;
        return true;
    }
    
    static box2<T> intersection( const box2<T> &lhs, const box2<T> &rhs ) {
        box2<T> result;
        
        if ( !intersect( lhs, rhs ) )
            return result;
        
        result.cmin = max( lhs.cmin, rhs.cmin );
        result.cmax = min( lhs.cmax, rhs.cmax );
        return result;
    }
    
    Vec2 cmin{ std::numeric_limits<T>::infinity() };
    Vec2 cmax{ -std::numeric_limits<T>::infinity() };
};

};

namespace pr {
    using Box2 = box2<float>;

class Path {
public:
    static const int GPU_BUFFER_ELEMENT_SIZE = sizeof(glm::vec4);
    template <class T> static uint32 numBufferElements() {
        return sizeof(T) / (sizeof(Vec4));
    }
    
    using CommandStorage = char;
    enum class Command : CommandStorage {
        MOVE_TO              = 'M',
        RELATIVE_MOVE_TO     = 'm',
        CLOSE                = 'Z',
        CLOSE_ALIAS          = 'z',

        LINE_TO              = 'L',
        RELATIVE_LINE_TO     = 'l',

        HORIZONTAL_LINE_TO           = 'H',
        RELATIVE_HORIZONTAL_LINE_TO  = 'h',
        VERTICAL_LINE_TO             = 'V',
        RELATIVE_VERTICAL_LINE_TO    = 'v',

        CIRCULAR_CCW_ARC_TO  = 'O',
        CIRCULAR_CW_ARC_TO   = 'o',

        ARC_TO               = 'A',
        RELATIVE_ARC_TO      = 'a',

        QUADRATIC_CURVE_TO                      = 'Q',
        RELATIVE_QUADRATIC_CURVE_TO             = 'q',
        SMOOTH_QUADRATIC_CURVE_TO               = 'T',
        RELATIVE_SMOOTH_QUADRATIC_CURVE_TO      = 't',

        CUBIC_CURVE_TO                      = 'C',
        RELATIVE_CUBIC_CURVE_TO             = 'c',
        SMOOTH_CUBIC_CURVE_TO               = 'S',
        RELATIVE_SMOOTH_CUBIC_CURVE_TO      = 's',
    };
    using Commands = std::vector<CommandStorage>;
    using Coords = std::vector<float>;

    enum class Attribute {
    	WEIGHT = 'w',
    	COLOR = 'c'
    };
   	using AttributeStorage = char;
 	using Attributes = std::vector<AttributeStorage>;

 	class Data {
 	public:
 		Commands commands;
 		Coords coords;
 		Attributes attributes;
 	};

 	enum Component {
 		PATH = 0,
        BOUND = 1,
  		//CONTROL = 1,
 		NUM_COMPONENTS,
 	};

    class Desc {
    public:
        bool isEditable = false;
        uint32 numSegments = 0;
        uint32 numSubpaths = 0;
    };
    
    static Path* create(const Desc* desc, const Data* data);

	virtual ~Path();


	const Data& getSource() const { return _source; }

    // If Path is editable, the authoring functions

    // insert a bunch of commands at the startCommand index in the current existing path command list
    // if startCommand is >= path.getNumCommands(), then the new commands are added at the end of the existing path.
    bool insertCommands(uint32 startCommand, const Data& data);

    // Erase a range of commands in the path
    // if startCommand is >= path.getNumCommands() nothing happens
    // if startCommand + numCommands is >= path.getNumCommands() then all the existing commands after (startCommand - 1) are deleted
    bool eraseCommands(uint32 startCommand, uint32 numCommands);

    // Update the coords values of a path for the range of commands [ startCOmmand, startCommand + nbCommands [
    // THe command coords provided must exactly match the actual path definition
    // This is the fast path to animate a path without modifying the command sequence
    bool updateCommandCoords(uint32 startCommand, uint32 numCommands, const Coords& coords);

    uint32 numCommands() const;
    uint32 numSubpaths() const;
    uint32 numSegments() const;
    uint32 numVertices() const;

    uint32 numGPUSubpaths() const;
    uint32 numGPUSegments() const;
    uint32 numGPUVertices() const;

    bool hasAttribs() const;

    bool isEditable() const;

public:
    
    class Vertex {
    public:
        using vector = std::vector<Vertex>;
        
        static Vec2 intToNormVec2(uint32 val) {
            return Vec2((((val & 0x0000FFFF) / 65535.0) * 2.0 - 1.0), ((((val & 0xFFFF0000) >> 16 ) / 65535.0 ) * 2.0 - 1.0) );
        }
        static uint32 normVec2ToInt(float x, float y) {
            return uint32(int( ( ( x + 1.f ) * 0.5f ) * 65535.f ) | int( int( ( ( y + 1.f ) * 0.5f ) * 65535.f ) << 16 ));
        }
        
        float x = 0.f;
        float y = 0.f;
        
        int   bin = 0;
        int   joint = 0;
        
        float aL = 0.0f;
        float al = 0.f;
        float sL = 0.f;
        float sl = 0.f;
        
#ifdef TEST_ARC
        float   type = 0.f;
        float   t0 = 1.0f;
        float   t1 = 0.f;
        float   t2 = 0.f;
#endif
        
        void setBin(float pbx, float pby) { bin = normVec2ToInt(pbx, pby); }
        void setJoint(float pjx, float pjy) { joint = normVec2ToInt(pjx, pjy); }
        
        Vec2 pos() const { return Vec2(x,y); }
        Vec2 binormal() const { return intToNormVec2((unsigned int) bin); }
        Vec2 tangent() const { Vec2 b = binormal(); return Vec2( b.y, -b.x ); }
    };
    
    class Segment {
    public:
        using vector = std::vector< Segment >;
        
        Vertex start;
        Vertex end;
        float   beginOffset = 0;
        float   endOffset = 0;
        float   subPathIndex = 0;
        float   spare2 = 0;
    };
    
    using AttributeVertices = std::vector< float >;
    
    class Format {
    public:
        using pointer = std::shared_ptr<Format>;
        using map = std::map< std::string, pointer >;
        
        Attributes _attribs;
        
        Format(const Attributes& attribs);
        
        const char* name() const;
        
        uint32 numAttribCoordsPerSegment() const;
        uint32 numAttribCoordsPerMoveTo() const;
        
        uint32 segmentBytesize() const;
        
    };
    
    class Subpath {
    public:
        using vector = std::vector< Subpath >;
        
        uint32    beginSegmentOffset = 0;
        uint32    nbSegments = 0;
        uint32    beginPathOffset = 0;
        uint32    spare1 = 0;
        Box2     bound;
    };
    
    struct PathInfo {
    public:
        using vector = std::vector< PathInfo >;
        
        uint32    beginSegmentOffset = 0;
        uint32    nbSegments = 0;
        uint32    beginSubpathOffset = 0;
        uint32    nbSubPaths = 0;
    };

    class CommandInfo {
    public:
        using vector = std::vector< CommandInfo >;
        
        uint32  beginSegmentOffset = 0;
        int16   command = 0;
        int16   nbSegments = 0;
    };


    class RenderData {
    public:
        Box2 bound;
        
        CommandInfo::vector commands;
        
        Segment::vector segments;
        
        AttributeVertices attributes;
        
        std::shared_ptr<Format> format;
        
        Subpath::vector subpaths;
    };
    
    class GPUData {
    public:
        gpu::BufferPointer _segmentsBuffer;
        uint32 numGPUSegments = 0;
        
        gpu::BufferPointer _attribsBuffer;
        uint32 numGPUAttribs = 0;
        
        gpu::BufferPointer _subpathsBuffer;
        uint32 numGPUSubpaths = 0;
    };
    
    class Builder {
    public:
        static int evalNumCoords(Command command);
        
        
        struct Point {
            int  idx = -1;
            Vec2 pos{ 0.0f };
            Vec2 tan{ 0.0f };
            Vec2 bin{ 0.0f };
            float plen = 0.0f;
            float slen = 0.0f;
        };
        
        struct Run{
            Point begin;
            Point end;
            Command command{ Command::CLOSE };
            Vec2 cp0{ 0.0f };
            float seglength{ 0.0f };
            
            Vec2 controlPoint() {
                if (    ( command == Command::QUADRATIC_CURVE_TO )
                    ||  ( command == Command::RELATIVE_QUADRATIC_CURVE_TO )
                    ||  ( command == Command::SMOOTH_QUADRATIC_CURVE_TO )
                    ||  ( command == Command::RELATIVE_SMOOTH_QUADRATIC_CURVE_TO )
                    
                    ||  ( command == Command::CUBIC_CURVE_TO )
                    ||  ( command == Command::RELATIVE_CUBIC_CURVE_TO )
                    ||  ( command == Command::SMOOTH_CUBIC_CURVE_TO )
                    ||  ( command == Command::RELATIVE_SMOOTH_CUBIC_CURVE_TO ) ) {
                    return cp0;
                } else {
                    return end.pos;
                }
            }
        };
        
        Point start;
        Run prev;
        Run next;
    
    
        static int evalNbCoords(Command command);
        static int evalJointTan(const Vec2& dir0, const Vec2& dir1, Vec2& bin0, Vec2& bin1, Vec2& joint0, Vec2& joint1);
        static int applyJointCoords(Path::Segment::vector& segments, int prevEnd, int nextBegin, const Vec2& prevEndTan, const Vec2& nextBeginTan, Vec2& prevEndBin, Vec2 nextBeginBin);
        static int finalizeVertex(Path::Segment::vector& segments, int& prevBegin, int& prevEnd, int nextBegin, int nextEnd,
                                                 Vec2& prevBeginTan, Vec2& prevEndTan, Vec2& nextBeginTan, Vec2& nextEndTan,
                                                 Vec2& prevEndBin, Vec2 nextBeginBin,
                                                 const Path::Vertex& ncp, const Path::Vertex& npep,
                                                 Path::Vertex& cp, Path::Vertex& pep);
        static int finalizeJoint(Path::Segment::vector& segments, Point& prevEnd, Point& nextBegin);
        
        static void pushSegment(Path::Segment::vector& segments, Point& pb, Point& pe, float seglength);
    
        bool closeSegment(Path::Segment::vector& segments);
        bool addMoveTo(Path::Segment::vector& segments, Command command, const float* lcoords);
        bool addLineTo(Path::Segment::vector& segments, Command command, const float* lcoords);
        bool addCircularArcTo(Path::Segment::vector& segments, Command command, const float* lcoords);
        bool addEllipticalArcTo(Path::Segment::vector& segments, Command command, const float* lcoords);
        bool addQuadraticCurveTo(Path::Segment::vector& segments, Command command, const float* lcoords);
        bool addCubicCurveTo(Path::Segment::vector& segments, Command command, const float* lcoords);
        uint32 addSegment(std::vector< int >& paths, Path::Segment::vector& segments, Command command, const float* lcoords);
        uint32 addSegmentAttribs(AttributeVertices& attribs, const Format* attribFormat, uint32 nbSegmentsAdded, const Path::Segment::vector& coords, Command command, const float* lcoords);

        int generatePathBuffer(std::vector< int >& beginEnds, Box2& bound, CommandInfo::vector& commands, Path::Segment::vector& segments, Subpath::vector& subpaths, AttributeVertices& attribs,
                               const Data* pData, const Format* attribFormat);

        bool buildPathBuffer(RenderData* path, const Desc* pDesc, const Data* pData);
        int rebuildPathBufferAddingCommands(RenderData* path, const Desc* pDesc, const Data* pAddedData);
        int updateCommandCoords(RenderData* path, uint32 startCommandNb, uint32 nbCommands, uint32 nbCoords, const float* pSysMemCoords, uint32* touchedSegmentOffset);
    
  };
    
protected:
    Path();
    
    Desc _desc;
    Data _source;
    RenderData _renderData;
    GPUData _gpuData;
    
    bool generateFormat(const Desc* pDesc, const Data* pData);
    
    bool generateVertexBuffer(const Desc* pDesc, const Data* pData);

};

/*
namespace intern {
     class Impl {

       box2f mBound;

        CommandInfo::vector mCommands;

        PathSegment::vector mSegments;
        gpu::BufferPointer mSegmentsBuffer;
        uint32 mNbGPUSegments;

        AttribVertex::vector mAttribs;
        gpu::BufferPointer mAttribsBuffer;
        uint32 mNbGPUAttribs;

        PathAttribFormat* mAttribsFormat;

        Subpath::vector mSubpaths;
        gpu::BufferPointer mSubpathsBuffer;
        uint32 mNbGPUSubpaths;


        static bool createSegmentsBuffer( ID3D11Device* pDevice, UINT nbSegments, const PathSegment* source, ID3D11Buffer** ppSegmentsBuffer, ID3D11ShaderResourceView** ppSegmentsBufferSRV );
        static bool createAttribsBuffer( ID3D11Device* pDevice, UINT nbSegments, const PathAttribFormat* attribFormat, const float* source, ID3D11Buffer** ppAttribsBuffer, ID3D11ShaderResourceView** ppAttribsBufferSRV );
        static bool createSubpathsBuffer( ID3D11Device* pDevice, UINT nbSubpaths, const Subpath* source, ID3D11Buffer** ppSubpathsBuffer, ID3D11ShaderResourceView** ppSubpathsBufferSRV );
        static bool createPathsBuffer( ID3D11Device* pDevice, UINT nbPaths, const PathInfo* source, ID3D11Buffer** ppPathsBuffer, ID3D11ShaderResourceView** ppPathsBufferSRV );

        static bool updateSegmentsBuffer( ID3D11DeviceContext* pDevice, UINT nbSegments, const PathSegment* source, ID3D11Buffer* pSegmentsBuffer );
        static bool updateAttribsBuffer( ID3D11DeviceContext* pDevice, UINT nbSegments, const PathAttribFormat* attribFormat, const float* source, ID3D11Buffer* pAttribsBuffer );
        static bool updateSubpathsBuffer( ID3D11DeviceContext* pDevice, UINT nbSubpaths, const Subpath* source, ID3D11Buffer* pSubpathsBuffer);

    private:
        bool generateFormat( P_ID3D11PathRenderingDevice* pDevice, const D3D11_PATH_DESC* pDesc, const D3D11_PATH_DATA* pData );

        bool generateVertexBuffer(const Path::Data* data, const );

        bool updateGPU( ID3D11Device* pDevice,  ID3D11DeviceContext* pContext );

        P_ID3D11Path();
        ~P_ID3D11Path();
        friend class ID3D11Path;
        friend class P_ID3D11PathRenderingDevice;

    };
}
  */
}
}
#endif