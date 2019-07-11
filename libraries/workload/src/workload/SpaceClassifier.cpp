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
    uint32_t numEvaluated = 0;
    if (numViews > 0) {
        out = _loadingRadius;
        auto theRadius = _loadingRadius;
        auto theReadyRadius = 1000.0f;
        _loadingOrigin = views[0].origin;
        auto theOrigin = _loadingOrigin;
        float maxDistancee { 0.0 };
        context->_space->accessProxies([&numEvaluated, &maxDistancee, &theReadyRadius, theRadius, theOrigin](Proxy::Vector& proxies) {
            uint32_t numProxies = (uint32_t)proxies.size();
            for (uint32_t i = 0; i < numProxies; ++i) {
                Proxy& proxy = proxies[i];
                if (proxy.phase == Phase::ON_HOLD) {
                    numEvaluated++;
                    glm::vec3 proxyCenter = glm::vec3(proxy.sphere);
                    float proxyRadius = proxy.sphere.w;

                    float touchDistance = proxyRadius + theRadius;
                    float touchDistance2 = touchDistance * touchDistance;
                    float centerDistance2 = distance2(proxyCenter, theOrigin);

                    float centerToProxyDistance = std::max(0.0f, sqrt(centerDistance2) - proxyRadius);
                    maxDistancee = std::max(centerToProxyDistance, maxDistancee);
                    if (theReadyRadius > centerToProxyDistance) {
                        theReadyRadius = centerToProxyDistance;
                    }

                    if (centerDistance2 < touchDistance2) {
                        proxy.phase = Phase::BEGIN_LOADING;
                    }
                }
            } 
        });
        if (numEvaluated) {
            _readyRadius = std::min(maxDistancee, theReadyRadius);
        }   
        _loadingRadius = _readyRadius + 1.0;

        auto config = std::static_pointer_cast<Config>(context->jobConfig);
        config->_readyRadius = _readyRadius;
        config->_loadingRadius = _loadingRadius;
        config->_loadingOrigin = _loadingOrigin;
        config->_numEvaluated = numEvaluated;
    }
}