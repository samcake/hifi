//
//  Phase.h
//  libraries/workload/src/workload
//
//  Created by Sam Gateau 2019.07.09
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#ifndef hifi_workload_Phase_h
#define hifi_workload_Phase_h

namespace workload {

class Phase {
public:
    using Type = uint8_t;

    enum Name : uint8_t
    {
        ON_HOLD = 0,
        BEGIN_LOADING,
        LOADING,
        DONE_LOADING,
        READY,
        UNKNOWN,
        INVALID,
    };

    static const uint8_t NUM_PHASES = UNKNOWN;

    static uint8_t computeNextIndex(uint8_t currentIndex);
};

inline uint8_t Phase::computeNextIndex(uint8_t currentIndex) {
    return (currentIndex < UNKNOWN ? currentIndex + 1 : ON_HOLD);
}

}  // namespace workload

#endif  // hifi_workload_Phase_h