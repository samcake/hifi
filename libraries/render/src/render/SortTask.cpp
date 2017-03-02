//
//  SortTask.cpp
//  render/src/render
//
//  Created by Sam Gateau on 2/2/16.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "SortTask.h"

#include <assert.h>
#include <queue>

#include "ShapePipeline.h"

#include <ViewFrustum.h>
#include <Profile.h>

using namespace render;

struct ItemBoundSort {
    float _centerDepth = 0.0f;
    float _nearDepth = 0.0f;
    float _farDepth = 0.0f;
    ItemID _id = 0;
    AABox _bounds;

    ItemBoundSort() {}
    ItemBoundSort(float centerDepth, float nearDepth, float farDepth, ItemID id, const AABox& bounds) : _centerDepth(centerDepth), _nearDepth(nearDepth), _farDepth(farDepth), _id(id), _bounds(bounds) {}
};

struct FrontToBackSort {
    bool operator() (const ItemBoundSort& left, const ItemBoundSort& right) {
        return (left._centerDepth < right._centerDepth);
    }
};

struct BackToFrontSort {
    bool operator() (const ItemBoundSort& left, const ItemBoundSort& right) {
        return (left._centerDepth > right._centerDepth);
    }
};

void render::depthSortItems(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, bool frontToBack, const ItemBounds& inItems, ItemBounds& outItems) {
    PROFILE_RANGE(render, "depthSort");
    assert(renderContext->args);
    assert(renderContext->args->hasViewFrustum());

    RenderArgs* args = renderContext->args;

    // Allocate and simply copy
    outItems.clear();
    outItems.reserve(inItems.size());

    // Make a local dataset of the center distance and closest point distance
    std::vector<ItemBoundSort> itemBoundSorts;
    itemBoundSorts.reserve(outItems.size());

    glm::vec3 eye = args->getViewFrustum().getPosition();
    for (const auto& itemDetails : inItems) {
        // use "distance squared" for speed: sorts the same as 'distance' but avoids a sqrt
        float distance2 = glm::distance2(eye, itemDetails.bound.calcCenter());
        itemBoundSorts.emplace_back(ItemBoundSort(distance2, distance2, distance2, itemDetails.id, itemDetails.bound));
    }

    // sort against Z
    if (frontToBack) {
        FrontToBackSort frontToBackSort;
        std::sort(itemBoundSorts.begin(), itemBoundSorts.end(), frontToBackSort);
    } else {
        BackToFrontSort  backToFrontSort;
        std::sort(itemBoundSorts.begin(), itemBoundSorts.end(), backToFrontSort);
    }

    // Finally once sorted result to a list of itemID
    for (auto& item : itemBoundSorts) {
       outItems.emplace_back(ItemBound(item._id, item._bounds));
    }
}

void PipelineSortShapes::run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ItemBounds& inItems, ShapeBounds& outShapes) {
    auto& scene = sceneContext->_scene;
    outShapes.clear();

    for (const auto& item : inItems) {
        auto key = scene->getItem(item.id).getShapeKey();
        auto outItems = outShapes.find(key);
        if (outItems == outShapes.end()) {
            outItems = outShapes.insert(std::make_pair(key, ItemBounds{})).first;
            outItems->second.reserve(inItems.size());
        }

        outItems->second.push_back(item);
    }

    for (auto& items : outShapes) {
        items.second.shrink_to_fit();
    }
}

void DepthSortShapes::run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ShapeBounds& inShapes, ShapeBounds& outShapes) {
    outShapes.clear();
    outShapes.reserve(inShapes.size());

    for (auto& pipeline : inShapes) {
        auto& inItems = pipeline.second;
        auto outItems = outShapes.find(pipeline.first);
        if (outItems == outShapes.end()) {
            outItems = outShapes.insert(std::make_pair(pipeline.first, ItemBounds{})).first;
        }

        depthSortItems(sceneContext, renderContext, _frontToBack, inItems, outItems->second);
    }
}

void DepthSortItems::run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ItemBounds& inItems, ItemBounds& outItems) {
    depthSortItems(sceneContext, renderContext, _frontToBack, inItems, outItems);
}


void PrioritySortItems::run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ItemBounds& inItems, ItemBounds& outItems) {
    PROFILE_RANGE(render, "prioritySort");
    RenderArgs* args = renderContext->args;
    assert(args);
    assert(args->hasViewFrustum());

    // allocate
    outItems.clear();
    outItems.reserve(inItems.size());

    // declare some tools
    class ItemPriority {
    public:
        ItemPriority(const ItemID& i, const AABox& b, float p) : id(i), bound(b), priority(p) {}
        bool operator<(const ItemPriority& other) const { return priority < other.priority; }
        ItemID id;
        AABox bound;
        float priority;
    };
    std::priority_queue<ItemPriority> sortedItems;

    // assemble priority queue
    glm::vec3 eye = args->getViewFrustum().getPosition();
    if (_strategy == (int)PrioritySortConfig::INV_DISTANCE) {
        for (const auto& itemDetails : inItems) {
            float distance = glm::distance(eye, itemDetails.bound.calcCenter()) - 0.5f * glm::length(itemDetails.bound.getScale());
            const float MIN_DISTANCE = 0.01f;
            sortedItems.emplace(ItemPriority(itemDetails.id, itemDetails.bound, 1.0f / glm::max(distance, MIN_DISTANCE)));
        }
    } else if (_strategy == (int)PrioritySortConfig::SOLID_ANGLE) {
        for (const auto& itemDetails : inItems) {
            float distance2 = glm::distance2(eye, itemDetails.bound.calcCenter()) + 0.0001f; // add 1cm^2 avoids divide by zero
            float priority = glm::length2(itemDetails.bound.getScale()) / distance2;
            sortedItems.emplace(ItemPriority(itemDetails.id, itemDetails.bound, priority));
        }
    } else { // WEIGHTED_SOLID_ANGLE
        const float PROXIMITY_BIAS_SQUARED = 150.0f * 150.0f;
        for (const auto& itemDetails : inItems) {
            float distance2 = glm::distance2(eye, itemDetails.bound.calcCenter()) + 0.0001f; // add 1cm^2 avoids divide by zero
            // the exponential envelope prefers nearby objects over distant ones (with same apparent size)
            float priority = expf(- distance2 / PROXIMITY_BIAS_SQUARED) * glm::length2(itemDetails.bound.getScale()) / distance2;
            sortedItems.emplace(ItemPriority(itemDetails.id, itemDetails.bound, priority));
        }
    }

    // transfer queue to list
    while (!sortedItems.empty()) {
        const ItemPriority& item = sortedItems.top();
        outItems.emplace_back(ItemBound(item.id, item.bound));
        sortedItems.pop();
    }
}

void TruncateItems::configure(const Config& config) {
    _numToKeep = config.numToKeep;
    const float MIN_BOUNDARY_WIDTH_FRACTION = 0.0f;
    const float MAX_BOUNDARY_WIDTH_FRACTION = 0.5f;
    float fraction = glm::clamp(config.boundaryWidthFraction, MIN_BOUNDARY_WIDTH_FRACTION, MAX_BOUNDARY_WIDTH_FRACTION);
    _boundaryWidth = (int)(fraction * (float) _numToKeep);
}

void TruncateItems::run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ItemBounds& inItems, ItemBounds& outItems) {
    int32_t numItems = (int32_t)inItems.size();

    if (_numToKeep > -1 && _numToKeep < numItems - _boundaryWidth) {
        ItemBounds::const_iterator first = inItems.begin();
        ItemBounds::const_iterator last = first + _numToKeep;
        outItems.assign(first, last);

        if (_boundaryWidth > 0) {
            // scan far side of boundary
            ItemIDSet::const_iterator notFound = _boundaryItems.end();
            _salvageIndices.clear();
            for (int32_t i = 0; i < _boundaryWidth; ++i) {
                if (_boundaryItems.find(inItems[_numToKeep + i].id) != notFound) {
                    // this item was visible last frame
                    // we'd like to keep it visible this frame if possible
                    _salvageIndices.push_back(_numToKeep + i);
                }
            }
            if (!_salvageIndices.empty()) {
                // scan near side of boundary
                int32_t j = 0;
                for (int32_t i = 0; i < _boundaryWidth / 2 && j < (int32_t)_salvageIndices.size(); ++i) {
                    int32_t k = _numToKeep - (i + 1);
                    if (_boundaryItems.find(outItems[k].id) == notFound) {
                        // this item was not visible at the boundary at last frame
                        // so we replace it with one we want to remain visible
                        outItems[k] = inItems[_salvageIndices[j]];
                        ++j;
                    }
                }
                for (; j < (int32_t)_salvageIndices.size(); ++j) {
                    // keep this object even though we're over budget
                    outItems.push_back(inItems[_salvageIndices[j]]);
                }
            }
            // remember the ids of items near the boundary that we will keep
            _boundaryItems.clear();
            for (int32_t i = _numToKeep - _boundaryWidth; i < (int32_t)outItems.size(); ++i) {
                _boundaryItems.insert(outItems[i].id);
            }
        }
    } else {
        outItems = inItems;
    }
}
