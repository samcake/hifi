//
//  Crowd.h
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 4/19/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#ifndef hifi_crowd_Crowd_h
#define hifi_crowd_Crowd_h

#include <AvatarData.h>

#include <memory>
#include <assert.h>
#include <render/IndexedContainer.h>


class AABox;

namespace crowd {

    using Bound = AABox;

    // the avatar in a crowd
    struct AvatarDesc {
        // a few attributes there
        AvatarSharedPointer _avatar;
    };

    using AvatarDescs = render::indexed_container::IndexedVector<AvatarDesc>;
    using Index = render::indexed_container::Index;

    template <class T> class AvatarElementBuffer;
    template <class T> class JointElementBuffer;


    class Flock;
    using FlockPointer = std::shared_ptr<Flock>;

    class Engine;
    using EnginePointer = std::shared_ptr<Engine>;

}

#endif // hifi_crowd_Crowd_h
