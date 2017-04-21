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
#include <render/IndexedContainer.h>

#include <AABox.h>
#include "ElementBuffer.h"

#include <AvatarData.h>

namespace crowd {

    // the avatar desc in a crowd
    // this is stored in a symem array managed in the Flock.
    struct AvatarDesc {
        // a few attributes there
        AvatarSharedPointer _avatar { nullptr };
        bool _isActive { false };
    };
    using AvatarDescs = render::indexed_container::IndexedVector<AvatarDesc>;

    // THe index of the avatar is assigned when an avatar is added to the Flock
    // It is the handle to the avatar in the Flock until its removed.
    using Index = render::indexed_container::Index;


    // The RootData struct is managed in a ElementBuffer
    // Contains the world space Bound of the avatar and the Root space transform expressed in world space.
    using Xform = glm::mat4x4;
    using Bound = AABox;
    struct RootData {
        Xform _transform;
        Bound _bound;
        float spare1;
        float spare2; // padd to be a multiple of 16 bytes
    };
    using RootBuffer = AvatarElementBuffer<RootData>;
    using RootBufferPointer = std::shared_ptr<RootBuffer>;

    // The flock is the container of all the avatar data of the crowd
    class Flock {
    public:
        Flock();
        virtual ~Flock();

        // Add / remove avatar and access their desc.
        // this will trigger the allocation/deallocation in subsequent data buffer as needed
        Index addAvatar(const AvatarDesc& avatar);
        void removeAvatar(Index id);
        Index getNumAvatars() const;
        const AvatarDesc& getAvatar(Index id) const;

        // Return the global capacity allocation
        // Aka the current max number of avatars that could be held in the various per avatar data structures
        Index getAvatarCapacity() const;

        // The element buffer containing one RootData for each Avatar.
        // indexable with the Avatar Index
        const RootBufferPointer& getRootBuffer() const { return _rootBuffer; }

    protected:
        AvatarDescs _avatars;

        RootBufferPointer _rootBuffer;

        // Really allocate per avatar data to accomodate for the specified number of avatars
        void resizeAvatarData(const Index numAvatars);

        // Reset an avatar with the current dec
        void resetAvatarData(Index id, const AvatarDesc& avatar);

    };
    using FlockPointer = std::shared_ptr<Flock>;
}

#endif // hifi_crowd_Flock_h
