//
//  SortTask.h
//  render/src/render
//
//  Created by Zach Pomerantz on 2/26/2016.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_render_SortTask_h
#define hifi_render_SortTask_h

#include "Engine.h"

namespace render {
    void depthSortItems(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, bool frontToBack, const ItemBounds& inItems, ItemBounds& outItems);

    class PipelineSortShapes {
    public:
        using JobModel = Job::ModelIO<PipelineSortShapes, ItemBounds, ShapeBounds>;
        void run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ItemBounds& inItems, ShapeBounds& outShapes);
    };

    class DepthSortShapes {
    public:
        using JobModel = Job::ModelIO<DepthSortShapes, ShapeBounds, ShapeBounds>;

        bool _frontToBack;
        DepthSortShapes(bool frontToBack = true) : _frontToBack(frontToBack) {}

        void run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ShapeBounds& inShapes, ShapeBounds& outShapes);
    };

    class DepthSortItems {
    public:
        using JobModel = Job::ModelIO<DepthSortItems, ItemBounds, ItemBounds>;

        bool _frontToBack;
        DepthSortItems(bool frontToBack = true) : _frontToBack(frontToBack) {}

        void run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ItemBounds& inItems, ItemBounds& outItems);
    };

	class PrioritySortConfig : public Job::Config {
	    Q_OBJECT
	    Q_PROPERTY(int sortStrategy READ getSortStrategy() WRITE setSortStrategy NOTIFY strategyChanged)
	public:
		enum SortStrategy {
			INV_DISTANCE,
			SOLID_ANGLE,
			WEIGHTED_SOLID_ANGLE
		};
	    int getSortStrategy() const { return (int)strategy; }
        void setSortStrategy(int s) { strategy = s; }
		int strategy { (int)INV_DISTANCE };
	signals:
	    void strategyChanged();
	};

    class PrioritySortItems {
    public:
		using Config = PrioritySortConfig;
        using JobModel = Job::ModelIO<PrioritySortItems, ItemBounds, ItemBounds, Config>;
        //using JobModel = Job::ModelIO<PrioritySortItems, ItemBounds, ItemBounds>;

        PrioritySortItems() {}

		void configure(const Config& config) { _strategy = config.strategy; }
        void run(const SceneContextPointer& sceneContext, const RenderContextPointer& renderContext, const ItemBounds& inItems, ItemBounds& outItems);
	protected:
		int _strategy;
    };
}

#endif // hifi_render_SortTask_h;
