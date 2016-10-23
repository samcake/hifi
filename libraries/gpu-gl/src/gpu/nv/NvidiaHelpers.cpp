//
//  NvidiaHelpers.cpp
//  libraries/gpu/src/gpu
//
//  Created by Sam Gateau on 10/22/2016.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "NvidiaHelpers.h"

using namespace gpu;
using namespace gpu::nv;

NvDriverPointer NvDriver::_singleton;
NvDriver::NvDriver()
#ifdef WIN32
    : _gpuHandle(0)
#endif
{
}

NvDriver::~NvDriver() {
}

void NvDriver::init() {
#ifdef WIN32 
      NvAPI_Status Status = NvAPI_Initialize();
    assert(Status == NVAPI_OK);

    NvPhysicalGpuHandle NvGpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { 0 };
    NvU32 NvGpuCount = 0;
    Status = NvAPI_EnumPhysicalGPUs(NvGpuHandles, &NvGpuCount);
    assert(Status == NVAPI_OK);
    assert(NvGpuCount != 0);
    _gpuHandle = NvGpuHandles[0];
#endif
}

NvDriverPointer NvDriver::get() {
    static std::once_flag once;
    std::call_once(once, [] {
        _singleton = std::make_shared<NvDriver>();
        _singleton->init();
    });
    return _singleton;
}

NvDriver::GPUMemInfo NvDriver::getGPUMemInfo() const {
    GPUMemInfo info;
#ifdef WIN32 
    NV_DISPLAY_DRIVER_MEMORY_INFO_V3 memInfo = { 0 };
    memInfo.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER_3;
    NvAPI_Status Status = NvAPI_GPU_GetMemoryInfo(_gpuHandle, &memInfo);
    assert(Status == NVAPI_OK);

    info.DedicatedVideoMemoryInMB = memInfo.dedicatedVideoMemory / 1024;
    info.AvailableDedicatedVideoMemoryInMB = memInfo.availableDedicatedVideoMemory / 1024;
    info.CurrentAvailableDedicatedVideoMemoryInMB = memInfo.curAvailableDedicatedVideoMemory / 1024;
#endif

    return info;
}