//
//  Flock.h
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 4/19/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#ifndef hifi_crowd_Flock_h
#define hifi_crowd_Flock_h

#include <glm/glm.hpp>
#include "Crowd.h"

#include <AABox.h>


namespace crowd {

    struct RootInfo {
        glm::mat4x4 _transform;
        Bound _bound;
        float spare1;
        float spare2;
    };

    class Flock {
    public:
        Flock();
        virtual ~Flock();

        Index addAvatar(const AvatarDesc& avatar);
        void removeAvatar(Index id);

        
    protected:
        AvatarDescs _avatars;

    };

}

#endif // hifi_crowd_Flock_h
