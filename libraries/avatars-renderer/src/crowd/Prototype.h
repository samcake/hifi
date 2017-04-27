//
//  Prototype.h
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 4/26/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#ifndef hifi_crowd_Prototype_h
#define hifi_crowd_Prototype_h

#include "Crowd.h"

namespace crowd {
namespace proto {

    class InjectAvatars {
    public:
        // Configuration
        class Config : public crowd::JobConfig {
        public:
            Config(bool enabled = true) : JobConfig(enabled) {}
        };

        // instantiate JobModel
        using JobModel = crowd::Job::Model<InjectAvatars, Config>;

        // Implement JobModel interface
        void configure(const Config& configuration) {}
        void run(const CrowdContextPointer& context);
    };

}
}

#endif // hifi_crowd_Prototype_h
