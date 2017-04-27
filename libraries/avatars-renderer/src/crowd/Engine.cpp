//
//  Engine.cpp
//  libraries/avatars-renderer/src/crowd
//
//  Created by Sam Gateau on 4/24/2017.
//  Copyright 2017 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "Engine.h"

#include <QtCore/QFile>
#include <QtCore/QUrl>

#include <PathUtils.h>

#include "../AvatarsRendererLogging.h"


#include "Prototype.h"

using namespace crowd;

class EngineTask {
public:
    using JobModel = Task::Model<EngineTask>;

    EngineTask() {}

    void build(JobModel& task, const Varying& in, Varying& out) {
    }
};

Engine::Engine() : Task("CrowdEngine", EngineTask::JobModel::create()),
    _context(std::make_shared<CrowdContext>())
{
    addJob<proto::InjectAvatars>("InjectAvatars");

}

void Engine::load() {
    auto config = getConfiguration();
    const QString configFile= "config/crowd.json";

    QUrl path(PathUtils::resourcesPath() + configFile);
    QFile file(path.toString());
    if (!file.exists()) {
        qWarning() << "Crowd engine configuration file" << path << "does not exist";
    } else if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Crowd engine configuration file" << path << "cannot be opened";
    } else {
        QString data = file.readAll();
        file.close();
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8(), &error);
        if (error.error == error.NoError) {
            config->setPresetList(doc.object());
            qCDebug(avatars_renderer) << "Crowd engine configuration file" << path << "loaded";
        } else {
            qWarning() << "Crowd engine configuration file" << path << "failed to load:" <<
                error.errorString() << "at offset" << error.offset;
        }
    }
}
