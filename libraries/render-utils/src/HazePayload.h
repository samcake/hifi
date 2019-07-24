//
//  HazePayload.h
//
//  Created by Sam Gateau on 23/7/2019.
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_HazePayload_h
#define hifi_HazePayload_h


#include <graphics/Haze.h>
#include <render/Item.h>
#include "HazeStage.h"

class HazePayload {
public:
    using Payload = render::Payload<HazePayload>;
    using Pointer = Payload::DataPointer;

    HazePayload();
    ~HazePayload();
    void render(RenderArgs* args);

    graphics::HazePointer editHaze() { _needUpdate = true; return _haze; }
    render::Item::Bound& editBound() { _needUpdate = true; return _bound; }

    void setVisible(bool visible) { _isVisible = visible; }
    bool isVisible() const { return _isVisible; }

protected:
    graphics::HazePointer _haze;
    render::Item::Bound _bound;
    HazeStagePointer _stage;
    HazeStage::Index _index { HazeStage::INVALID_INDEX };
    bool _needUpdate { true };
    bool _isVisible{ true };
};

namespace render {
    template <> const ItemKey payloadGetKey(const HazePayload::Pointer& payload);
    template <> const Item::Bound payloadGetBound(const HazePayload::Pointer& payload);
    template <> void payloadRender(const HazePayload::Pointer& payload, RenderArgs* args);
}


class FogOfWarConfig : public render::Job::Config {
    Q_OBJECT
    Q_PROPERTY(float range MEMBER range NOTIFY dirty)
    Q_PROPERTY(float backgroundBlend MEMBER backgroundBlend NOTIFY dirty)
public:

    FogOfWarConfig() : render::Job::Config(
    ) {}

    float range{ -1.0 };
    float backgroundBlend{ 0.0f };

signals:
    void dirty();

protected:
};

class FogOfWar {
public:
    using Input = render::ItemBounds;
    using Config = FogOfWarConfig;
    using JobModel = render::Job::ModelI<FogOfWar, Input, Config>;

    FogOfWar() {}

    void configure(const Config& config);
    void run(const render::RenderContextPointer& context, const Input& input);
    HazePayload _haze;
    bool _doIt { false };

};

#endif