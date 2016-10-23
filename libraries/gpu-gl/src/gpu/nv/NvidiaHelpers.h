//
//  NvidiaHelpers.h
//  libraries/gpu/src/gpu
//
//  Created by Sam Gateau on 10/22/2016.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#ifndef hifi_gpu_nv_NvidiaHelpers_h
#define hifi_gpu_nv_NvidiaHelpers_h

#include <QtCore/QLoggingCategory>

#ifdef WIN32 
#include <nvapi.h>
#endif

#include <gl/Config.h>

#include <gpu/Forward.h>
#include <gpu/Context.h>


namespace gpu { namespace nv {

class NvDriver;
using NvDriverPointer = std::shared_ptr<NvDriver>;

class NvDriver {
    static NvDriverPointer _singleton;
    void init();
#ifdef WIN32 
    NvPhysicalGpuHandle _gpuHandle;
#endif

public:
    NvDriver();
    ~NvDriver();
    static NvDriverPointer get();

    struct GPUMemInfo {
        uint32_t DedicatedVideoMemoryInMB;
        uint32_t AvailableDedicatedVideoMemoryInMB;
        uint32_t CurrentAvailableDedicatedVideoMemoryInMB;
    };

    GPUMemInfo getGPUMemInfo() const;
};

} }

#endif
