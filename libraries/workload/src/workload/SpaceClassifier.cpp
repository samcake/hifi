//
//  SpaceClassifier.cpp
//  libraries/workload/src/workload
//
//  Created by Andrew Meadows 2018.02.21
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "SpaceClassifier.h"

#include "ViewTask.h"
#include "RegionState.h"

using namespace workload;

void PerformSpaceTransaction::configure(const Config& config) {

}
void PerformSpaceTransaction::run(const WorkloadContextPointer& context) {
    context->_space->enqueueFrame();
    context->_space->processTransactionQueue();
}

void SpaceClassifierTask::build(JobModel& model, const Varying& in, Varying& out) {
    model.addJob<AssignSpaceViews >("assignSpaceViews", in);
    model.addJob<PerformSpaceTransaction>("updateSpace");
    model.addJob<UpdatePhase>("updatePhase", in);
    const auto regionTrackerOut = model.addJob<RegionTracker>("regionTracker");
    const auto regionChanges = regionTrackerOut.getN<RegionTracker::Outputs>(1);
    model.addJob<RegionState>("regionState", regionChanges);
    out = regionTrackerOut;
}

void UpdatePhase::configure(const Config& config) {

}

void UpdatePhase::run(const WorkloadContextPointer& context, const Inputs& in, Outputs& out) {
    auto views = in;
    uint32_t numViews = (uint32_t) views.size();
    uint32_t numEvaluated[Phase::NUM_PHASES] = { 0, 0, 0, 0, 0};
    if (numViews > 0) {
        out = _loadingRadius;
        auto theRadius = _loadingRadius;
        auto theReadyRadius = 1000.0f;
        _loadingOrigin = views[0].origin;
        auto theOrigin = _loadingOrigin;
        float maxBeginLoadingDistance{ 0.0 };
        float maxDoneLoadingDistance{ 0.0 };
        context->_space->accessProxies([&numEvaluated, &maxBeginLoadingDistance, &maxDoneLoadingDistance,  &theReadyRadius, theRadius, theOrigin](Proxy::Vector& proxies) {
            uint32_t numProxies = (uint32_t)proxies.size();
            for (uint32_t i = 0; i < numProxies; ++i) {
                Proxy& proxy = proxies[i];
                if (proxy.phase == Phase::ON_HOLD) {
                    numEvaluated[Phase::ON_HOLD]++;
                    proxy._padding = 0;

                    glm::vec3 proxyCenter = glm::vec3(proxy.sphere);
                    float proxyRadius = proxy.sphere.w;

                    float touchDistance = proxyRadius + theRadius;
                    float touchDistance2 = touchDistance * touchDistance;
                    float centerDistance2 = distance2(proxyCenter, theOrigin);

                    float centerToProxyDistance = std::max(0.0f, sqrt(centerDistance2) - proxyRadius);
                    maxBeginLoadingDistance = std::max(centerToProxyDistance, maxBeginLoadingDistance);
                    if (theReadyRadius > centerToProxyDistance) {
                        theReadyRadius = centerToProxyDistance;
                    }

                    if (centerDistance2 < touchDistance2) {
                        proxy._padding = 0;
                        proxy.phase = Phase::BEGIN_LOADING;
                    }
                } else if (proxy.phase == Phase::BEGIN_LOADING) {
                    numEvaluated[Phase::BEGIN_LOADING]++;
                    
                    if (proxy._padding < 255) {
                        proxy._padding ++;
                    } else {
                        proxy._padding = 0;
                        proxy.phase = Phase::LOADING;
                    }
                } else if (proxy.phase == Phase::LOADING) {
                    numEvaluated[Phase::LOADING]++;

                    if (proxy._padding < 255) {
                        proxy._padding++;
                    } else {
                        proxy._padding = 0;
                        proxy.phase = Phase::DONE_LOADING;
                    }
                } else if (proxy.phase == Phase::DONE_LOADING) {
                    numEvaluated[Phase::DONE_LOADING]++;

                    if (proxy._padding < 255) {
                        proxy._padding++;
                        continue;
                    } else {
                        proxy._padding = 0;
                    }

                    glm::vec3 proxyCenter = glm::vec3(proxy.sphere);
                    float proxyRadius = proxy.sphere.w;

                    float centerDistance2 = distance2(proxyCenter, theOrigin);

                    float centerToProxyDistance = std::max(0.0f, sqrt(centerDistance2) - proxyRadius);
                    maxDoneLoadingDistance = std::max(centerToProxyDistance, maxDoneLoadingDistance);
                    if (theReadyRadius > centerToProxyDistance) {
                        theReadyRadius = centerToProxyDistance;
                    }
                    proxy.phase = Phase::READY;
                }
            } 
        });
        
        int numChanged = 0;
        for (auto n : numEvaluated) {
            numChanged += n;
        }

        if (numChanged) {
            _readyRadius = std::min(maxBeginLoadingDistance, theReadyRadius);
        }  
        _loadingRadius = _readyRadius + 1.0;

        auto config = std::static_pointer_cast<Config>(context->jobConfig);
        config->_readyRadius = _readyRadius;
        config->_loadingRadius = _loadingRadius;
        config->_loadingOrigin = _loadingOrigin;
        config->_numEvaluated = numChanged;
    }
}