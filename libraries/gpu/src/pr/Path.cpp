//
//  Path.cpp
//  libraries/gpu/src/path
//
//  Created by Sam Gateau on 10/15/2015.
//  Copyright 2015 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "Path.h"

#include <sstream>

using namespace gpu;
using namespace pr;

//--------------------------------------------------------------------------------------
// Path Attrib Format
//--------------------------------------------------------------------------------------

Path::Path::Format::Format(const Path::Attributes& attribs) {
    _attribs = attribs;
    _attribs.push_back('\n');
}

const char* Path::Format::name() const {
    return _attribs.data();
}

uint32 Path::Format::numAttribCoordsPerSegment() const {
    uint32 num = 0;
    for (auto attrib : _attribs) {
        switch ((Path::Attribute) attrib) {
        case Attribute::WEIGHT:
            num += 2;
            break;
        default:
            break;
        }
    }
    return num;
}

uint32 Path::Format::numAttribCoordsPerMoveTo() const {
    uint32 num = 0;
    for (auto attrib : _attribs) {
        switch ((Path::Attribute) attrib) {
            case Attribute::WEIGHT:
            default:
                break;
        }
    }
    return num;
}

uint32 Path::Format::segmentBytesize() const {
    uint32 bytesize = numAttribCoordsPerSegment() * sizeof(float);
    return bytesize;
}



#define NB_CURVE_SUB_SEGMENTS 16.f

//--------------------------------------------------------------------------------------
// P_ID3D11Path
//--------------------------------------------------------------------------------------
/*
bool P_ID3D11Path::createSegmentsBuffer(ID3D11Device* pDevice, UINT nbSegments, const Path::Segment* source, ID3D11Buffer** ppSegmentsBuffer, ID3D11ShaderResourceView** ppSegmentsBufferSRV)
{
    if ((*ppSegmentsBuffer))
        (*ppSegmentsBuffer)->Release();
    (*ppSegmentsBuffer) = 0;

    if ((*ppSegmentsBufferSRV))
        (*ppSegmentsBufferSRV)->Release();
    (*ppSegmentsBufferSRV) = 0;

    if (nbSegments > 0)
    {
        D3D11_BUFFER_DESC bd;
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.ByteWidth = nbSegments * sizeof(Path::Segment);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA bda;
        bda.pSysMem = (void*) source;
        bda.SysMemPitch = bd.ByteWidth;
        bda.SysMemSlicePitch = bd.ByteWidth;
        D3D11_SUBRESOURCE_DATA* pbda = (source ? &bda : 0);

        HRESULT hr = pDevice->CreateBuffer(&bd, pbda, ppSegmentsBuffer);
        if (hr != S_OK)
        {
            return false;
        }

        if ((*ppSegmentsBuffer))
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
            srvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srvd.Buffer.FirstElement = 0;

            srvd.Buffer.NumElements = nbSegments * Path::Segment::nbBufferElements();

            HRESULT hr = pDevice->CreateShaderResourceView((*ppSegmentsBuffer), &srvd, ppSegmentsBufferSRV);
            if (hr != S_OK)
            {
                return false;
            }
        }
    }
    return true;
}
bool P_ID3D11Path::createAttribsBuffer(ID3D11Device* pDevice, UINT nbSegments, const AttributeFormat* attribFormat, const float* source, ID3D11Buffer** ppAttribsBuffer, ID3D11ShaderResourceView** ppAttribsBufferSRV)
{
    if ((*ppAttribsBuffer))
        (*ppAttribsBuffer)->Release();
    (*ppAttribsBuffer) = 0;

    if ((*ppAttribsBufferSRV))
        (*ppAttribsBufferSRV)->Release();
    (*ppAttribsBufferSRV) = 0;

    if (attribFormat && (nbSegments > 0))
    {
        D3D11_BUFFER_DESC bd;
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.ByteWidth = nbSegments * attribFormat->segmentBytesize();
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA bda;
        bda.pSysMem = (void*) source;
        bda.SysMemPitch = bd.ByteWidth;
        bda.SysMemSlicePitch = bd.ByteWidth;
        D3D11_SUBRESOURCE_DATA* pbda = (source ? &bda : 0);

        HRESULT hr = pDevice->CreateBuffer(&bd, pbda, ppAttribsBuffer);
        if (hr != S_OK)
        {
            return false;
        }

        if ((*ppAttribsBuffer))
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
            srvd.Format = DXGI_FORMAT_R32_FLOAT;
            srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srvd.Buffer.FirstElement = 0;

            srvd.Buffer.NumElements = bd.ByteWidth / sizeof(float);

            HRESULT hr = pDevice->CreateShaderResourceView((*ppAttribsBuffer), &srvd, ppAttribsBufferSRV);
            if (hr != S_OK)
            {
                return false;
            }
        }
    }
    return true;
}

bool P_ID3D11Path::createSubpathsBuffer(ID3D11Device* pDevice, UINT nbSubpaths, const Subpath* source, ID3D11Buffer** ppSubpathsBuffer, ID3D11ShaderResourceView** ppSubpathsBufferSRV)
{
    if ((*ppSubpathsBuffer))
        (*ppSubpathsBuffer)->Release();
    (*ppSubpathsBuffer) = 0;

    if ((*ppSubpathsBufferSRV))
        (*ppSubpathsBufferSRV)->Release();
    (*ppSubpathsBufferSRV) = 0;

    if (nbSubpaths > 0)
    {
        D3D11_BUFFER_DESC bd;
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.ByteWidth = nbSubpaths * sizeof(Subpath);
#ifdef PATHGPUCOPY
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.CPUAccessFlags = 0;
            bd.MiscFlags = 0;
#else
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.CPUAccessFlags = 0;
            bd.MiscFlags = 0;
    //        bd.Usage = D3D11_USAGE_DYNAMIC;
    //        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //        bd.MiscFlags = 0;
#endif
        bd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA bda;
        bda.pSysMem = (void*) source;
        bda.SysMemPitch = bd.ByteWidth;
        bda.SysMemSlicePitch = bd.ByteWidth;
        D3D11_SUBRESOURCE_DATA* pbda = (source ? &bda : 0);

        HRESULT hr = pDevice->CreateBuffer(&bd, pbda, ppSubpathsBuffer);
        if (hr != S_OK)
        {
            return false;
        }

        if ((*ppSubpathsBuffer))
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
            srvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srvd.Buffer.FirstElement = 0;

            srvd.Buffer.NumElements = nbSubpaths * Subpath::nbBufferElements();

            HRESULT hr = pDevice->CreateShaderResourceView((*ppSubpathsBuffer), &srvd, ppSubpathsBufferSRV);
            if (hr != S_OK)
            {
                return false;
            }
        }
    }
    return true;
}

bool P_ID3D11Path::createPathsBuffer(ID3D11Device* pDevice, UINT nbPaths, const PathInfo* source, ID3D11Buffer** ppPathsBuffer, ID3D11ShaderResourceView** ppPathsBufferSRV)
{
    if ((*ppPathsBuffer))
        (*ppPathsBuffer)->Release();
    (*ppPathsBuffer) = 0;

    if ((*ppPathsBufferSRV))
        (*ppPathsBufferSRV)->Release();
    (*ppPathsBufferSRV) = 0;

    if (nbPaths > 0)
    {
        D3D11_BUFFER_DESC bd;
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.ByteWidth = nbPaths * sizeof(PathInfo);
#ifdef PATHGPUCOPY
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.CPUAccessFlags = 0;
            bd.MiscFlags = 0;
#else
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.CPUAccessFlags = 0;
            bd.MiscFlags = 0;
    //        bd.Usage = D3D11_USAGE_DYNAMIC;
    //        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //        bd.MiscFlags = 0;
#endif
        bd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA bda;
        bda.pSysMem = (void*) source;
        bda.SysMemPitch = bd.ByteWidth;
        bda.SysMemSlicePitch = bd.ByteWidth;
        D3D11_SUBRESOURCE_DATA* pbda = (source ? &bda : 0);

        HRESULT hr = pDevice->CreateBuffer(&bd, pbda, ppPathsBuffer);
        if (hr != S_OK)
        {
            return false;
        }

        if ((*ppPathsBuffer))
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
            srvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            srvd.Buffer.FirstElement = 0;

            srvd.Buffer.NumElements = nbPaths * PathInfo::nbBufferElements();

            HRESULT hr = pDevice->CreateShaderResourceView((*ppPathsBuffer), &srvd, ppPathsBufferSRV);
            if (hr != S_OK)
            {
                return false;
            }
        }
    }
    return true;
}

bool P_ID3D11Path::updateSegmentsBuffer(ID3D11DeviceContext* pDevice, UINT nbSegments, const Path::Segment* source, ID3D11Buffer* pSegmentsBuffer)
{
    D3D11_BOX box;
    box.front = 0;
    box.back = 1;
    box.top = 0;
    box.bottom = 1;
    box.left = 0;
    box.right = nbSegments * sizeof(Path::Segment);


    pDevice->UpdateSubresource(pSegmentsBuffer, 0, &box , source, 0, 0);

    return true;
}

bool P_ID3D11Path::updateAttribsBuffer(ID3D11DeviceContext* pDevice, UINT nbSegments, const AttributeFormat* attribFormat, const float* source, ID3D11Buffer* pAttribsBuffer)
{
    D3D11_BOX box;
    box.front = 0;
    box.back = 1;
    box.top = 0;
    box.bottom = 1;
    box.left = 0;
    box.right = nbSegments * attribFormat->segmentBytesize();


    pDevice->UpdateSubresource(pAttribsBuffer, 0, &box , source, 0, 0);

    return true;
}

bool P_ID3D11Path::updateSubpathsBuffer(ID3D11DeviceContext* pDevice, UINT nbSubpaths, const Subpath* source, ID3D11Buffer* pSubpathsBuffer)
{
    D3D11_BOX box;
    box.front = 0;
    box.back = 1;
    box.top = 0;
    box.bottom = 1;
    box.left = 0;
    box.right = nbSubpaths * sizeof(Subpath);


    pDevice->UpdateSubresource(pSubpathsBuffer, 0, &box , source, 0, 0);

    return true;
}
*/
//--------------------------------------------------------------------------------------



Path* Path::create(const Desc* pDesc, const Data* pData) {
    auto path = new Path();
    path->generateFormat(pDesc, pData);
    path->generateVertexBuffer(pDesc, pData);
    return path;
}

bool Path::generateFormat(const Desc* pDesc, const Data* pData) {
    if (pData && pData->attributes.size()) {
      //  _renderData.format = createPathFormat(pData->pAttribs, pData->NbAttribs);
        return true;
    } else {
        _renderData.format.reset();
        return true;
    }
}

bool Path::generateVertexBuffer(const Desc* pDesc, const Data* pData) {
    // grab the desc
    memcpy(&_desc, pDesc, sizeof(Desc));

    if (_desc.isEditable) {
        if (_desc.numSegments <= 0)
            _desc.numSegments = 100;

        if (_desc.numSubpaths <= 0)
            _desc.numSubpaths = 5;
    }

    if (pData && pData->commands.size() && pData->coords.size()) {
        Path::Builder pb;
        bool result = pb.buildPathBuffer(&_renderData, &_desc, pData);
        if (!result)
            return false;
    }

    return true;
}
/*
bool P_ID3D11Path::updateGPU(ID3D11Device* pDevice,  ID3D11DeviceContext* pContext)
{
    if (mDesc.IsEditable)
    {
        if (!pDevice && !pContext)
        {
            return false;
        }

        UINT alloc = ((nbSegments() / mDesc.PreAllocateNbSegments) + 1) * mDesc.PreAllocateNbSegments; 
        if (mNbGPUSegments < alloc)
        {
            mNbGPUSegments = alloc;
            P_ID3D11Path::createSegmentsBuffer(pDevice, mNbGPUSegments,0, &mSegmentsBuffer, &mSegmentsBufferSRV);
        }
        P_ID3D11Path::updateSegmentsBuffer(pContext, nbSegments(), mSegments.data(), mSegmentsBuffer);

        if (mSegmentsBuffer && mAttribsFormat && !mAttribs.empty())
        {
            if (mNbGPUAttribs < alloc)
            {
                mNbGPUAttribs = alloc;
                P_ID3D11Path::createAttribsBuffer(pDevice, mNbGPUAttribs, mAttribsFormat, 0, &mAttribsBuffer, &mAttribsBufferSRV);
            }
            P_ID3D11Path::updateAttribsBuffer(pContext, nbSegments(), mAttribsFormat, mAttribs.data(), mAttribsBuffer);
        }

        alloc = ((nbSubpaths() / mDesc.PreAllocateNbSubpaths) + 1) * mDesc.PreAllocateNbSubpaths; 
        if (mNbGPUSubpaths < alloc)
        {
            mNbGPUSubpaths = alloc;
            P_ID3D11Path::createSubpathsBuffer(pDevice, mNbGPUSubpaths, mSubpaths.data(), &mSubpathsBuffer, &mSubpathsBufferSRV);
        }
        P_ID3D11Path::updateSubpathsBuffer(pContext, nbSubpaths(), mSubpaths.data(), mSubpathsBuffer);

    }
    else
    {
        if (!pDevice)
        {
            return false;
        }

        // Segments
        if (nbSegments())
        {
            mNbGPUSegments = nbSegments();
            P_ID3D11Path::createSegmentsBuffer(pDevice, mNbGPUSegments, mSegments.data(), &mSegmentsBuffer, &mSegmentsBufferSRV);
        }
        // Attribs
        if (mSegmentsBuffer && mAttribsFormat && !mAttribs.empty())
        {
            mNbGPUAttribs = nbSegments();
            P_ID3D11Path::createAttribsBuffer(pDevice, mNbGPUAttribs, mAttribsFormat, mAttribs.data(), &mAttribsBuffer, &mAttribsBufferSRV);
        }

        // Subpaths
        if (nbVertices() && nbSubpaths())
        {
            mNbGPUSubpaths = nbSubpaths();
            P_ID3D11Path::createSubpathsBuffer(pDevice, mNbGPUSubpaths, mSubpaths.data(), &mSubpathsBuffer, &mSubpathsBufferSRV);
        }
    }

    return true;
}
*/

uint32 Path::numCommands() const {
    return _source.commands.size();
}
uint32 Path::numSubpaths() const {
    return _renderData.subpaths.size();
}
uint32 Path::numSegments() const {
    return _renderData.segments.size();
}
uint32 Path::numVertices() const {
    return numSegments() * 2;
}

uint32 Path::numGPUSubpaths() const {
    return _gpuData.numGPUSubpaths;
}
uint32 Path::numGPUSegments() const {
    return _gpuData.numGPUSegments;
}
uint32 Path::numGPUVertices() const {
    return _gpuData.numGPUSegments * 2;
}

bool Path::hasAttribs() const {
    return !(_renderData.format);
}

bool Path::isEditable() const {
    return _desc.isEditable;
}
/*
HRESULT Path::insertCommands(P_ID3D11PathRenderingDeviceContext* pPRContext, uint32 startCommand, const D3D11_PATH_DATA *pAddedData)
{
    if (startCommand >= nbCommands())
    {
        PathBuilder pb;
        bool result = pb.rebuildPathBufferAddingCommands(this, &mDesc, pAddedData);

        if (!result)
            return false;

        updateGPU(pPRContext->m_pPPRDevice->m_pD3D11Device, pPRContext->m_pD3D11DeviceContext);
    }

    return S_OK;
}

HRESULT P_ID3D11Path::eraseCommands(P_ID3D11PathRenderingDeviceContext* pContext, uint32 startCommand, uint32 nbCommands)
{

    return S_OK;
}


HRESULT P_ID3D11Path::updateCommandCoords(P_ID3D11PathRenderingDeviceContext* pContext, uint32 startCommand, uint32 pNbCommands, uint32 nbCoords, const float* pSysMemCoords)
{
    if ((startCommand >= nbCommands()) || (startCommand + pNbCommands > nbCommands()))
        return S_FALSE;

    uint32 touchedSegmentOffset;
    PathBuilder pb;
    uint32 nbSegmentsTouched = pb.updateCommandCoords(this, startCommand, pNbCommands, nbCoords, pSysMemCoords, &touchedSegmentOffset);

     updateGPU(pContext->m_pPPRDevice->m_pD3D11Device, pContext->m_pD3D11DeviceContext);

/*
    CommandInfo* command = & mCommands[startCommand];
    uint32 startTouchedSegment = command->beginSegmentOffset;
    uint32 currentSegment = startTouchedSegment;

    PathBuilder pb;
    bool result = pb.buildPathBuffer(this, pDesc, pData);
    if (!result)
        return false;

    uint32 currentCoordNb = 0;
    for (uint32 c = 0; c < pNbCommands; c++)
    {
        for (uint32 s =0; s< command->nbSegments; s++)
        {
            uint32 commandNbCoords = PathBuilder::evalNbCoords(command->command);
            if (currentCoordNb + commandNbCoords <= nbCoords)
            {

            }


            currentSegment++;
        }
    }

   // if (!result)
   //     return false;

   // updateGPU(pPRContext->m_pPPRDevice->m_pD3D11Device, pPRContext->m_pD3D11DeviceContext);
   */
 /*   return S_OK;
}
*/


#define PATHGPUCOPY 1
/*
//--------------------------------------------------------------------------------------
// P_ID3D11MultiPath
//--------------------------------------------------------------------------------------
P_ID3D11MultiPath::P_ID3D11MultiPath() :
    mMultiNbSegments(0),
    mMultiNbSubpaths(0),
    mMultiSegmentsBuffer(0),
    mMultiSegmentsBufferSRV(0),
    mMultiAttribsBuffer(0),
    mMultiAttribsBufferSRV(0),
    mMultiSubpathsBuffer(0),
    mMultiSubpathsBufferSRV(0),
    mMultiPathsBuffer(0),
    mMultiPathsBufferSRV(0)
{
}

P_ID3D11MultiPath::~P_ID3D11MultiPath()
{
    uint32 count;

    if (mMultiSegmentsBufferSRV)
       count = mMultiSegmentsBufferSRV->Release();
    mMultiSegmentsBufferSRV = 0;
    if (mMultiSegmentsBuffer)
       count = mMultiSegmentsBuffer->Release();
    mMultiSegmentsBuffer = 0;

    if (mMultiAttribsBufferSRV)
       count = mMultiAttribsBufferSRV->Release();
    mMultiAttribsBufferSRV = 0;
    if (mMultiAttribsBuffer)
       count = mMultiAttribsBuffer->Release();
    mMultiAttribsBuffer = 0;

    if (mMultiSubpathsBufferSRV)
       count = mMultiSubpathsBufferSRV->Release();
    mMultiSubpathsBufferSRV = 0;
    if (mMultiSubpathsBuffer)
       count = mMultiSubpathsBuffer->Release();
    mMultiSubpathsBuffer = 0;

    if (mMultiPathsBufferSRV)
       count = mMultiPathsBufferSRV->Release();
    mMultiPathsBufferSRV = 0;
    if (mMultiPathsBuffer)
       count = mMultiPathsBuffer->Release();
    mMultiPathsBuffer = 0;
}

AttributeFormat* P_ID3D11MultiPath::pathAttriFormat()
{
    if (mPaths.empty())
        return 0;
    else
        return mPaths[0]->m_pPriv->mAttribsFormat;
}


P_ID3D11MultiPath* P_ID3D11MultiPath::create(ID3D11Device* pDevice, const D3D11_MULTIPATH_DESC* pDesc, const D3D11_MULTIPATH_DATA* pData)
{
    if (!(pData->NbPaths && pData->ppPaths))
        return 0;


    P_ID3D11MultiPath* path = new P_ID3D11MultiPath();

    path->mMultiNbSubpaths = 0;
    path->mMultiNbSegments = 0;

    PathInfo pi;

    for (int p = 0; p < pData->NbPaths; p++)
    {
        ID3D11Path* sp = pData->ppPaths[ p ];

        pi.beginSegmentOffset += pi.nbSegments;
        pi.beginSubpathOffset += pi.nbSubPaths;


        // only increment if sp exists
        if (sp)
        {
            sp->AddRef();
            path->mMultiNbSegments += sp->m_pPriv->nbSegments();
            path->mMultiNbSubpaths += sp->m_pPriv->nbSubpaths();

            pi.nbSegments = sp->m_pPriv->nbSegments();
            pi.nbSubPaths = sp->m_pPriv->nbSubpaths();
        }
        else
        {
            pi.nbSegments = pi.nbSubPaths = 0;
        }

        path->mPaths.push_back(sp);
        path->mMultiPaths.push_back(pi);
    }


    path->updateGPU(pDevice);

    return path;
}

bool P_ID3D11MultiPath::updateGPU(ID3D11Device* pDevice)
{
    if (nbPaths() <= 1)
        return false;

     // so far so good so let's just display a bunch of something now
    //if (!mMultiSegmentsBuffer || (mMultiNbSegments < nbSegments()))
    if (nbSegments())
        P_ID3D11Path::createSegmentsBuffer(pDevice, nbSegments(), 0, &mMultiSegmentsBuffer, &mMultiSegmentsBufferSRV);
    // Attribs
    if (nbSegments() && pathAttriFormat())
        P_ID3D11Path::createAttribsBuffer(pDevice, nbSegments(), pathAttriFormat(), 0, &mMultiAttribsBuffer, &mMultiAttribsBufferSRV);
    // Subpaths
    if (nbVertices() && nbSubpaths())
        P_ID3D11Path::createSubpathsBuffer(pDevice, nbSubpaths(), 0, &mMultiSubpathsBuffer, &mMultiSubpathsBufferSRV);
    // Paths
    if (nbVertices() && nbSubpaths())
        P_ID3D11Path::createPathsBuffer(pDevice, nbPaths(), mMultiPaths.data(), &mMultiPathsBuffer, &mMultiPathsBufferSRV);

    ID3D11DeviceContext* deviceContext;
    pDevice->GetImmediateContext(&deviceContext);

    // Here the buffer are big enough, can copy the values from the different paths bound
    uint32 dstSegmentsOffset = 0;
    uint32 dstAttribsOffset = 0;
    uint32 dstSubpathsOffset = 0;

    for (uint32 p = 0; p < mPaths.size(); p++)
    {
        if (mPaths[p])
        {
            P_ID3D11Path* path = mPaths[p]->m_pPriv;

            if (path->nbSegments())
            {
                if (path->mSegmentsBuffer)
                {
                    deviceContext->CopySubresourceRegion(mMultiSegmentsBuffer, 0, dstSegmentsOffset, 0, 0, path->mSegmentsBuffer, 0, 0);
                }
                else
                {
                    D3D11_BOX srcBox;
                    srcBox.back = 0;
                    srcBox.front = 0;
                    srcBox.top = 1;
                    srcBox.bottom = 1;
                    srcBox.left = dstSegmentsOffset;
                    srcBox.right = dstSegmentsOffset + path->nbSegments() * sizeof(Path::Segment);
                    deviceContext->UpdateSubresource(mMultiSegmentsBuffer, 0, &srcBox, (void*) path->mSegments.data(), 0, 0);
                }
                dstSegmentsOffset += path->nbSegments() * sizeof(Path::Segment);
            }

            if (pathAttriFormat() && path->hasAttribs())
            {
                if (path->mAttribsBuffer)
                {
                    deviceContext->CopySubresourceRegion(mMultiAttribsBuffer, 0, dstAttribsOffset, 0, 0, path->mAttribsBuffer, 0, 0);
                }
                else
                {
                    D3D11_BOX srcBox;
                    srcBox.back = 0;
                    srcBox.front = 0;
                    srcBox.top = 1;
                    srcBox.bottom = 1;
                    srcBox.left = dstAttribsOffset;
                    srcBox.right = dstAttribsOffset + path->nbSegments() * pathAttriFormat()->segmentBytesize();
                    deviceContext->UpdateSubresource(mMultiAttribsBuffer, 0, &srcBox, (void*) path->mAttribs.data(), 0, 0);
                }
                dstAttribsOffset += path->nbSegments() * pathAttriFormat()->segmentBytesize();
            }

            if (path->nbSubpaths())
            {
                if (path->mSubpathsBuffer)
                {
                    deviceContext->CopySubresourceRegion(mMultiSubpathsBuffer, 0, dstSubpathsOffset, 0, 0, path->mSubpathsBuffer, 0, 0);
                }
                else
                {
                    D3D11_BOX srcBox;
                    srcBox.back = 0;
                    srcBox.front = 0;
                    srcBox.top = 1;
                    srcBox.bottom = 1;
                    srcBox.left = dstSubpathsOffset;
                    srcBox.right = dstSubpathsOffset + path->nbSubpaths() * sizeof(Subpath);
                    deviceContext->UpdateSubresource(mMultiSubpathsBuffer, 0, &srcBox, (void*) path->mSubpaths.data(), 0, 0);
                }
                dstSubpathsOffset += path->nbSubpaths() * sizeof(Subpath);
            }
        }
    }

    deviceContext->Release();
    return true;
}

uint32 P_ID3D11MultiPath::nbSubpaths()
{
    return mMultiNbSubpaths;
}

uint32 P_ID3D11MultiPath::nbSegments()
{
    return mMultiNbSegments;
}

uint32 P_ID3D11MultiPath::nbVertices()
{
    return nbSegments() / 2;
}

uint32 P_ID3D11MultiPath::nbPaths()
{
    return mPaths.size();
}
*/