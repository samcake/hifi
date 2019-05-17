//
//  Created by Bradley Austin Davis on 2019/05/14
//  Copyright 2013-2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//


#include "PerformanceScriptingInterface.h"

#include "../Application.h"

std::once_flag PerformanceScriptingInterface::registry_flag;

PerformanceScriptingInterface::PerformanceScriptingInterface() {
    std::call_once(registry_flag, [] {
        qmlRegisterType<PerformanceScriptingInterface>("PerformanceEnums", 1, 0, "RefreshRate");
    });
}

void PerformanceScriptingInterface::setRefreshRateProfile(RefreshRateProfile refreshRateProfile) {
    qApp->getRefreshRateManager().setRefreshRateProfile((RefreshRateManager::RefreshRateProfile)refreshRateProfile);
}

PerformanceScriptingInterface::RefreshRateProfile PerformanceScriptingInterface::getRefreshRateProfile() const {
    return (PerformanceScriptingInterface::RefreshRateProfile)qApp->getRefreshRateManager().getRefreshRateProfile();
}

int PerformanceScriptingInterface::getActiveRefreshRate() const {
    return qApp->getRefreshRateManager().getActiveRefreshRate();
}

RefreshRateManager::UXMode PerformanceScriptingInterface::getUXMode() const {
    return qApp->getRefreshRateManager().getUXMode();
}

RefreshRateManager::RefreshRateRegime PerformanceScriptingInterface::getRefreshRateRegime() const {
    return qApp->getRefreshRateManager().getRefreshRateRegime();
}
