//
//  Engine.h
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 4/24/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#pragma once
#ifndef hifi_crowd_Engine_h
#define hifi_crowd_Engine_h

#include <task/Task.h>

namespace crowd {
   class Flock;
   using FlockPointer = std::shared_ptr<Flock>;

   class CrowdContext : public task::JobContext {
    public:
        virtual ~CrowdContext() {}

        FlockPointer _flock;
    };
    using CrowdContextPointer = std::shared_ptr<CrowdContext>;

    Task_DeclareTypeAliases(CrowdContext)

    
    // The crowd engine holds all crowd tasks, and is itself a crowd task.
    // State flows through tasks to jobs via the crowd job contexts -
    // the engine should not be known from its jobs.
    class Engine : public Task {
    public:

        Engine();
        ~Engine() = default;

        // Load any persisted settings, and set up the presets
        // This should be run after adding all jobs, and before building ui
        void load();

        // Register the flock
        void registerFlock(const FlockPointer& flock) { _context->_flock = flock; }

        // acces the CrowdContext
        CrowdContextPointer getContext() const { return _context; }

        // Perform a frame
        // Must have a flock registered and a context set
        void run() { assert(_context); assert(_context->_flock);  Task::run(_context); }

    protected:
        CrowdContextPointer _context;
    
        void run(const CrowdContextPointer& context) override { assert(_context);  Task::run(_context); }
    };
    using EnginePointer = std::shared_ptr<Engine>;
}

#endif // hifi_crowd_Engine_h
