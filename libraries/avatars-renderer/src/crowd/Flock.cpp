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

Flock::Flock() :
    _avatars(),
    _rootBuffer(std::make_shared<RootBuffer>())
{

}

Flock::~Flock() {

}

Index Flock::getAvatarCapacity() const {
    return _avatars.getNumAllocatedIndices();
}

Index Flock::getNumAvatars() const {
    return _avatars.getNumElements();
}

const AvatarDesc& Flock::getAvatar(Index id) const {
    return _avatars.get(id);
}

Index Flock::addAvatar(const AvatarDesc& avatar) {
    auto avatarId = _avatars.newElement(avatar);

    // After adding the avatar spot in the index table, let s allocate in subsequent per avatar datastructures
    resizeAvatarData(_avatars.getNumAllocatedIndices());

    // and then reset the new avatar
    resetAvatarData(avatarId, avatar);

    return avatarId;
}

void Flock::removeAvatar(Index id) {
    _avatars.freeElement(id);
}

void Flock::resizeAvatarData(const Index numAvatars) {
    
    _rootBuffer->resize(numAvatars);
}

void Flock::resetAvatarData(Index id, const AvatarDesc& avatar) {
    avatar._avatar->getPosition();
    avatar._avatar->getOrientation();
    avatar._avatar->getScale();
}
