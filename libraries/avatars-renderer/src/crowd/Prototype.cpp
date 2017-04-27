//
//  Prototype.cpp
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 4/26/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "Prototype.h"

using namespace crowd;
using namespace crowd::proto;

void InjectAvatars::run(const CrowdContextPointer& context) {

    auto flock = context->_flock;
    auto numAvatars = flock->getNumAvatars();
    int numInjected = 10 - numAvatars;
    if (numInjected > 0) {
        for (int i = 0; i < numInjected; numInjected++) {
            AvatarDesc newAvatar;

            auto id = flock->addAvatar(newAvatar);
        }
    }

}

