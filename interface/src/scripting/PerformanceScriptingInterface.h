//
//  Created by Bradley Austin Davis on 2019/05/14
//  Copyright 2013-2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#pragma once
#ifndef hifi_PerformanceScriptingInterface_h
#define hifi_PerformanceScriptingInterface_h

#include <mutex>

#include <QObject>

#include "../RefreshRateManager.h"


class PerformanceScriptingInterface : public QObject {
    Q_OBJECT
public:
    // Must match RefreshRateManager enums
    enum RefreshRateProfile {
        ECO = RefreshRateManager::RefreshRateProfile::ECO,
        INTERACTIVE = RefreshRateManager::RefreshRateProfile::INTERACTIVE,
        REALTIME = RefreshRateManager::RefreshRateProfile::REALTIME,
    };
    Q_ENUM(RefreshRateProfile)

   
    PerformanceScriptingInterface();
    ~PerformanceScriptingInterface() = default;

public slots:
    void setRefreshRateProfile(RefreshRateProfile refreshRateProfile);
    RefreshRateProfile getRefreshRateProfile() const;

    int getActiveRefreshRate() const;
    RefreshRateManager::UXMode getUXMode() const;
    RefreshRateManager::RefreshRateRegime getRefreshRateRegime() const;


private:
    static std::once_flag registry_flag;
};

#endif // header guard
