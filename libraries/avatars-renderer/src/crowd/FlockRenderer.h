//
//  FlockRenderer.h
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 5/19/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#ifndef hifi_crowd_FlockRenderer_h
#define hifi_crowd_FlockRenderer_h

#include <render/Item.h>
#include "Crowd.h"

namespace crowd {

    class FlockDebugPayload {
    public:
        using Payload = render::Payload<FlockDebugPayload>;
        using Pointer = Payload::DataPointer;

        FlockDebugPayload(const FlockPointer& flock);
        ~FlockDebugPayload();
        void render(RenderArgs* args);

        render::Item::Bound& editBound() { return _bound; }

        void setVisible(bool visible) { _isVisible = visible; }
        bool isVisible() const { return _isVisible; }

        render::ShapeKey FlockDebugPayload::getShapeKey() const;

    protected:
        render::Item::Bound _bound;
        FlockPointer _flock;


        bool _needUpdate { true };
        bool _isVisible { true };


        const gpu::PipelinePointer getRootPipeline();
        gpu::PipelinePointer _rootPipeline;
        int _colorLocation { -1 };
    };



}

namespace render {
    template <> const ItemKey payloadGetKey(const crowd::FlockDebugPayload::Pointer& payload);
    template <> const ShapeKey shapeGetShapeKey(const crowd::FlockDebugPayload::Pointer& payload);
    template <> const ItemKey payloadGetKey(const crowd::FlockDebugPayload::Pointer& payload);
    template <> void payloadRender(const crowd::FlockDebugPayload::Pointer& payload, RenderArgs* args);
    template <> int payloadGetLayer(const crowd::FlockDebugPayload::Pointer& payload);
}


namespace crowd {
    class FlockSceneUpdate {
    public:
        // Configuration
        class Config : public crowd::JobConfig {
        public:
            Config(bool enabled = true) : JobConfig(enabled) {}
        };

        // instantiate JobModel
        using JobModel = crowd::Job::Model<FlockSceneUpdate, Config>;

        // Implement JobModel interface
        void configure(const Config& configuration) {}
        void run(const CrowdContextPointer& context);


        render::ItemIDs _renderItems;
    };
}

#endif // hifi_crowd_FlockRenderer_h
