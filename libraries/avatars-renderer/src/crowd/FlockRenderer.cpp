//
//  FlockRenderer.cpp
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 5/19/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "FlockRenderer.h"

#include <gpu/Batch.h>
#include <render/Scene.h>

using namespace crowd;

#include <drawFlockRoots_vert.h>
#include <drawFlockRoots_frag.h>

namespace render {
    template <> const ItemKey payloadGetKey(const FlockDebugPayload::Pointer& payload) {
        ItemKey::Builder builder;
        builder.withTypeLight();
        if (!payload->isVisible()) {
            builder.withInvisible();
        }
        builder.withTypeMeta();
        builder.withLayered();

        return builder.build();
    }

    template <> const ShapeKey shapeGetShapeKey(const FlockDebugPayload::Pointer& payload) {
        return payload->getShapeKey();
    }

    template <> const Item::Bound payloadGetBound(const FlockDebugPayload::Pointer& payload) {
        return payload->editBound();
    }
    template <> void payloadRender(const FlockDebugPayload::Pointer& payload, RenderArgs* args) {
        payload->render(args);
    }
    template <> int payloadGetLayer(const FlockDebugPayload::Pointer& payload) {
        return 1;
    }

}

FlockDebugPayload::FlockDebugPayload(const FlockPointer& flock) :
    _flock(flock)
{
    _bound.setBox(glm::vec3(0.0f), glm::vec3(30.0f));
}


FlockDebugPayload::~FlockDebugPayload() {
}


render::ShapeKey FlockDebugPayload::getShapeKey() const {

    render::ShapeKey::Builder builder;
    builder.withOwnPipeline();
    return builder.build();
}


const gpu::PipelinePointer FlockDebugPayload::getRootPipeline() {
    if (!_rootPipeline) {
        auto vs = gpu::Shader::createVertex(std::string(drawFlockRoots_vert));
        auto ps = gpu::Shader::createPixel(std::string(drawFlockRoots_frag));
        gpu::ShaderPointer program = gpu::Shader::createProgram(vs, ps);

        gpu::Shader::BindingSet slotBindings;
        gpu::Shader::makeProgram(*program, slotBindings);

        _colorLocation = program->getUniforms().findLocation("inColor");

        auto state = std::make_shared<gpu::State>();
        state->setDepthTest(true, false, gpu::LESS_EQUAL);
        state->setBlendFunction(true,
            gpu::State::SRC_ALPHA, gpu::State::BLEND_OP_ADD, gpu::State::INV_SRC_ALPHA,
            gpu::State::DEST_ALPHA, gpu::State::BLEND_OP_ADD, gpu::State::ZERO);

        _rootPipeline = gpu::Pipeline::create(program, state);
    }

    return _rootPipeline;
}

void FlockDebugPayload::render(RenderArgs* args) {
    assert(args);
    if (!_flock) {
        return;
    }


    auto numAvatars = _flock->getNumAvatars();

   auto batch = args->_batch;

    // Setup projection
   batch->setModelTransform(Transform());

    // Bind program
   batch->setPipeline(getRootPipeline());

   glm::vec4 color(glm::vec3(0.0f), -(float)numAvatars);
   batch->_glUniform4fv(_colorLocation, 1, (const float*)(&color));


   batch->setResourceBuffer(0, (_flock->getRootBuffer()->_buffer));

    static const int NUM_VERTICES_PER_CUBE = 24;
    batch->draw(gpu::LINES, NUM_VERTICES_PER_CUBE * numAvatars, 0);

}


void FlockSceneUpdate::run(const CrowdContextPointer& context) {
    auto flock = context->_flock;

    if (_renderItems.empty()) {
        render::Transaction transaction;

        auto itemID = context->_scene->allocateID();

        auto data = std::make_shared<FlockDebugPayload>(flock);
        auto payload = std::make_shared<FlockDebugPayload::Payload>(data);

        transaction.resetItem(itemID, payload);

        context->_scene->enqueueTransaction(transaction);
        _renderItems.push_back(itemID);
    }


}

