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

uint32_t get1DNoiseUint(int posX, uint32_t seed = 0) {
    const uint32_t BIT_NOISE1 = 0x68E31DA4;
    const uint32_t BIT_NOISE2 = 0xB5297A4D;
    const uint32_t BIT_NOISE3 = 0x1B56C4E9;

    uint32_t mangledBits = (uint32_t) posX;
    mangledBits *= BIT_NOISE1;
    mangledBits += seed;
    mangledBits ^= (mangledBits >> 8);
    mangledBits += BIT_NOISE2;
    mangledBits ^= (mangledBits << 8);
    mangledBits *= BIT_NOISE3;
    mangledBits ^= (mangledBits >> 8);
    return mangledBits;
}
float get1DNoiseNormalizedFloat(int posX, uint32_t seed = 0) {
    return (float) ((double)(get1DNoiseUint(posX, seed)) / (double)std::numeric_limits<uint32_t>::max());
}

glm::vec3 getRandomPos(int index, const glm::vec3& scale = glm::vec3(1.0f)) {
    
    glm::vec3 pos(
        get1DNoiseNormalizedFloat(3 * index),
        get1DNoiseNormalizedFloat(3 * index + 1),
        get1DNoiseNormalizedFloat(3 * index + 2));
    return pos * scale;
}


void InjectAvatars::run(const CrowdContextPointer& context) {

    auto flock = context->_flock;
    auto numAvatars = flock->getNumAvatars();
    int numInjected = 10 - numAvatars;
    if (numInjected > 0) {
        glm::vec3 scale(20.0f, 0.f, 20.0f);
        for (int i = 0; i < numInjected; i++) {
            AvatarDesc newAvatar;


            newAvatar._avatar = std::make_shared<AvatarData>();


            glm::vec3 pos = getRandomPos(i, scale);
            newAvatar._avatar->setPosition(pos);

            auto id = flock->addAvatar(newAvatar);
        }
    }

}

