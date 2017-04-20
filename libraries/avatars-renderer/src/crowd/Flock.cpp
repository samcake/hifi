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
#include "Flock.h"

using namespace crowd;

Flock::Flock() {

}

Flock::~Flock() {

}

Index Flock::addAvatar(const AvatarDesc& avatar) {
    auto avatarId = _avatars.newElement(avatar);



    return avatarId;
}

void Flock::removeAvatar(Index id) {
    _avatars.freeElement(id);
}

