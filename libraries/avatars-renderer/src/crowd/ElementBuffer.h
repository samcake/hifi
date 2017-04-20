//
//  ElementBuffer.h
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 4/19/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#ifndef hifi_crowd_ElementBuffer_h
#define hifi_crowd_ElementBuffer_h

#include "Crowd.h"

#include <gpu/Buffer.h>

namespace crowd {

    template (class T) class ElementBuffer {
    public:
        ElementBuffer();
        virtual ~ElementBuffer();

        Index addAvatar(const AvatarDesc& avatar);
        void removeAvatar(Index id);

        
    protected:
        
    };

}

#endif // hifi_crowd_Flock_h
