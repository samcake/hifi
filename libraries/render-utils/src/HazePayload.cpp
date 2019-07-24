//
//  HazePayload.cpp
//
//  Created by Sam Gateau on 23/7/2019.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "HazePayload.h"


#include <gpu/Batch.h>

//#include "LightStage.h"
#include "BackgroundStage.h"
#include "HazeStage.h"
//#include "BloomStage.h"


namespace render {
    template <> const ItemKey payloadGetKey(const HazePayload::Pointer& payload) {
        ItemKey::Builder builder;
       // builder.withTypeLight();
        builder.withTagBits(ItemKey::TAG_BITS_ALL);
        if (payload) {
            if (!payload->isVisible()) {
                builder.withInvisible();
            }
        }

        return builder.build();
    }

    template <> const Item::Bound payloadGetBound(const HazePayload::Pointer& payload) {
        if (payload) {
            return payload->editBound();
        }
        return render::Item::Bound();
    }
    template <> void payloadRender(const HazePayload::Pointer& payload, RenderArgs* args) {
        if (args) {
            if (payload) {
                payload->render(args);
            }
        }
    }
}

HazePayload::HazePayload() :
    _haze(std::make_shared<graphics::Haze>())
{
}


HazePayload::~HazePayload() {
    if (!HazeStage::isIndexInvalid(_index)) {
        if (_stage) {
            _stage->removeHaze(_index);
        }
    }
}

void HazePayload::render(RenderArgs* args) {
    if (!_stage) {
        _stage = args->_scene->getStage<HazeStage>();
        assert(_stage);
    }
    // Do we need to allocate the haze in the stage ?
    if (HazeStage::isIndexInvalid(_index)) {

        _index = _stage->addHaze(_haze);
        _needUpdate = false;
    }
    // Need an update ?
    if (_needUpdate) {
        _needUpdate = false;
    }
    
    if (isVisible()) {
        // FInally, push the haze visible in the frame
        _stage->_currentFrame.pushHaze(_index);

#ifdef WANT_DEBUG
        Q_ASSERT(args->_batch);
        gpu::Batch& batch = *args->_batch;
        batch.setModelTransform(getTransformToCenter());
        DependencyManager::get<GeometryCache>()->renderWireSphere(batch, 0.5f, 15, 15, glm::vec4(color, 1.0f));
#endif
    }
}

void FogOfWar::configure(const Config& config) {

    _doIt = (config.range >= 0.0f);
    _haze.editHaze()->setHazeActive(_doIt);

    _haze.editHaze()->setHazeRangeFactor(graphics::Haze::convertHazeRangeToHazeRangeFactor(std::abs(config.range)));

    _haze.editHaze()->setHazeBackgroundBlend(config.backgroundBlend);
}

void FogOfWar::run(const render::RenderContextPointer& context, const Input& input) {

    if (_doIt) {
        auto hazeStage = context->_scene->getStage<HazeStage>();
        assert(hazeStage);
        hazeStage->_currentFrame.clear();

        _haze.render(context->args);
    }
}
