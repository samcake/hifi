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

#include <algorithm>

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
    _expansionSpeed = config.getExpansionSpeed();
    _fakeLoadingTime = config.getFakeLoadingTime();
}

void UpdatePhase::run(const WorkloadContextPointer& context, const Inputs& in, Outputs& out) {
   
    auto startTime = std::chrono::high_resolution_clock::now();
    auto durationChrono = startTime - _lastTime;
    _lastTime = startTime;
    auto duration = std::chrono::duration<double, std::milli>(durationChrono).count() * 0.001;


    auto views = in;
    uint32_t numViews = (uint32_t) views.size();
    glm::ivec4 numEvaluated{ 0 };
    if (numViews > 0) {

        // Grab current loading origin from view
        _loadingOrigin = views[0].origin;

        auto loadingOrigin = _loadingOrigin;
        auto loadingRadius = _loadingRadius;
        auto readyRadius = _readyRadius;

        
        glm::vec2 distanceRanges[] = { { 1000.0f, 0.0f},  { 1000.0f, 0.0f},  { 1000.0f, 0.0f},  { 1000.0f, 0.0f} };

        glm::ivec4 fakeLoadingTempo = {
            0,
            std::min(255, (int)floor(_fakeLoadingTime * 1)),
            std::min(255, (int)floor(_fakeLoadingTime * 10)),
            std::min(255, (int)floor(_fakeLoadingTime * 1)),
        };

        context->_space->accessProxies([fakeLoadingTempo, &numEvaluated, &distanceRanges, loadingRadius, loadingOrigin](Proxy::Vector& proxies) {
            uint32_t numProxies = (uint32_t)proxies.size();
            for (uint32_t i = 0; i < numProxies; ++i) {
                Proxy& proxy = proxies[i];
                if (proxy.phase != Phase::READY) {
                    // proxy distance to origin
                    glm::vec3 proxyCenter = glm::vec3(proxy.sphere);
                    float proxyRadius = proxy.sphere.w;
                    float centerDistance2 = distance2(proxyCenter, loadingOrigin);
                    float originToProxyDistance = std::max(0.0f, sqrtf(centerDistance2) - proxyRadius);

                    // Per phase counters
                    numEvaluated[proxy.phase]++;
                    auto& distanceRange = distanceRanges[proxy.phase];
                    distanceRange.x = std::min(originToProxyDistance, distanceRange.x);
                    distanceRange.y = std::max(originToProxyDistance, distanceRange.y);

                    // Evolve proxy phase progression
                    switch (proxy.phase) {
                        case Phase::ON_HOLD: {
                            proxy._padding = 0;

                            float touchDistance = proxyRadius + loadingRadius;
                            float touchDistance2 = touchDistance * touchDistance;
                            if (centerDistance2 < touchDistance2) {
                                proxy._padding = 0;
                                proxy.phase = Phase::BEGIN_LOADING;
                            }
                        } break;
                        case Phase::BEGIN_LOADING: {
                            if (proxy._padding < fakeLoadingTempo[Phase::BEGIN_LOADING]) {
                                proxy._padding++;
                            } else {
                                proxy._padding = 0;
                                proxy.phase = Phase::LOADING;
                            }
                        } break;
                        case Phase::LOADING: {
                            if (proxy._padding < fakeLoadingTempo[Phase::LOADING]) {
                                proxy._padding++;
                            }
                            else {
                                proxy._padding = 0;
                                proxy.phase = Phase::DONE_LOADING;
                            }
                        } break;
                        case Phase::DONE_LOADING: {
                            if (proxy._padding < fakeLoadingTempo[Phase::DONE_LOADING]) {
                                proxy._padding++;
                            }
                            else {
                                proxy._padding = 0;
                                proxy.phase = Phase::READY;
                            }
                        } break;
                    }
                }
            } 
        });
        


        
        glm::vec2 onHoldDistanceRange( distanceRanges[Phase::ON_HOLD] );

        glm::vec2 loadingDistanceRange( distanceRanges[Phase::BEGIN_LOADING] );
        int numLoading = numEvaluated[Phase::BEGIN_LOADING] + numEvaluated[Phase::LOADING] + numEvaluated[Phase::DONE_LOADING];
        if (numLoading) {
            loadingDistanceRange.x = std::min(distanceRanges[Phase::LOADING].x, loadingDistanceRange.x);
            loadingDistanceRange.y = std::max(distanceRanges[Phase::LOADING].y, loadingDistanceRange.y);

            loadingDistanceRange.x = std::min(distanceRanges[Phase::DONE_LOADING].x, loadingDistanceRange.x);
            loadingDistanceRange.y = std::max(distanceRanges[Phase::DONE_LOADING].y, loadingDistanceRange.y);
        }

        // Eval the next reaady radius
        auto newReadyRadius = std::min(onHoldDistanceRange.x, loadingDistanceRange.x);
        auto maxChange = (float)duration * _expansionSpeed;
        auto readyRadiusChange = newReadyRadius - readyRadius;
        if (readyRadiusChange > 0) {
            if (readyRadiusChange < maxChange) {
                _readyRadius = newReadyRadius;
            } else {
                _readyRadius += maxChange;
            }
        } else {
            _readyRadius = newReadyRadius;
        }


        _loadingRadius = _readyRadius + maxChange;



        out = _loadingRadius;

        auto config = std::static_pointer_cast<Config>(context->jobConfig);
        config->_readyRadius = _readyRadius;
        config->_loadingRadius = _loadingRadius;
        config->_loadingOrigin = _loadingOrigin;
        config->_onHoldRange = distanceRanges[Phase::ON_HOLD];
        config->_loadingRange = loadingDistanceRange;
        config->_numEvaluatedPerPhase = numEvaluated;
    }
}