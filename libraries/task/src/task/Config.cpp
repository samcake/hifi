//
//  Config.cpp
//  render/src/task
//
//  Created by Zach Pomerantz on 1/21/2016.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "Config.h"

#include <QtCore/QThread>

#include <shared/QtHelpers.h>

#include "Task.h"

using namespace task;

std::string Annotation::buildJSONString() const {
    std::stringstream jsonString;
    jsonString << "{ " << build() << " }";
    return jsonString.str();
}

Annotation& Annotation::beginProp(const std::string& name, const std::string& type) {
    if (!_firstProp) {
        _stream << ", ";
    }
    _firstProp = false;

    _stream << "\"" << name << "\":{ \"type\":\"" << type << "\"";
    return (*this);
}

Annotation& Annotation::propScalar(const std::string& name, float min, float max, const std::string& unit) {
    beginProp(name, "scalar");
    _stream << ", \"min\":" << min << ", \"max\":" << max << ", \"unit\":\"" << unit << "\" }";
    return (*this);
}

Annotation& Annotation::propEnum(const std::string& name, std::initializer_list<std::string> enums) {
    beginProp(name, "enum");
    _stream << ", \"enums\":[ ";
    int i = 0;
    for (auto value : enums) {
        _stream << "\"" << value << "\"";
        i++;
        if (i < enums.size()) {
            _stream << ", ";
        }
    }
    _stream << " ] }";
    return (*this);
}


JobConfig::~JobConfig() {
    
}

void JobConfig::setEnabled(bool enable) {
    if (_isEnabled != enable) {
        _isEnabled = enable;
        emit dirtyEnabled();
    }
}

void JobConfig::setPresetList(const QJsonObject& object) {
    for (auto it = object.begin(); it != object.end(); it++) {
        JobConfig* child = findChild<JobConfig*>(it.key(), Qt::FindDirectChildrenOnly);
        if (child) {
            child->setPresetList(it.value().toObject());
        }
    }
}

QString JobConfig::getPropertyAnnotations() const {
    Annotation annotation;
    QString annotationJSONString{ appendConfigPropAnnotation(annotation).buildJSONString().c_str() };
    return annotationJSONString;
}


Annotation& JobConfig::appendConfigPropAnnotation(Annotation& annotation) const {
    // nothing specific
    return annotation;
}

void JobConfig::connectChildConfig(QConfigPointer childConfig, const std::string& name) {
    childConfig->setParent(this);
    childConfig->setObjectName(name.c_str());

    // Connect loaded->refresh
    QObject::connect(childConfig.get(), SIGNAL(loaded()), this, SLOT(refresh()));
    static const char* DIRTY_SIGNAL = "dirty()";
    if (childConfig->metaObject()->indexOfSignal(DIRTY_SIGNAL) != -1) {
        // Connect dirty->refresh if defined
        QObject::connect(childConfig.get(), SIGNAL(dirty()), this, SLOT(refresh()));
        QObject::connect(childConfig.get(), SIGNAL(dirtyEnabled()), this, SLOT(refresh()));
    }
}

void JobConfig::transferChildrenConfigs(std::shared_ptr<JobConfig> source) {
    if (!source) {
        return;
    }
    // Transfer children to the new configuration
    auto children = source->children();
    for (auto& child : children) {
        child->setParent(this);
        QObject::connect(child, SIGNAL(loaded()), this, SLOT(refresh()));
        static const char* DIRTY_SIGNAL = "dirty()";
        if (child->metaObject()->indexOfSignal(DIRTY_SIGNAL) != -1) {
            // Connect dirty->refresh if defined
            QObject::connect(child, SIGNAL(dirty()), this, SLOT(refresh()));
            QObject::connect(child, SIGNAL(dirtyEnabled()), this, SLOT(refresh()));
        }
    }
}

void JobConfig::refresh() {
    if (QThread::currentThread() != thread()) {
        BLOCKING_INVOKE_METHOD(this, "refresh");
        return;
    }

    _jobConcept->applyConfiguration();
}

//TaskConfig* JobConfig::getRootConfig(const std::string& jobPath, std::string& jobName) const {
JobConfig* JobConfig::getRootConfig(const std::string& jobPath, std::string& jobName) const {
    //   TaskConfig* root = const_cast<TaskConfig*> (this);
    JobConfig* root =/* dynamic_cast<TaskConfig*> */(const_cast<JobConfig*>(this));

    std::list<std::string> tokens;
    std::size_t pos = 0, sepPos;
    while ((sepPos = jobPath.find_first_of('.', pos)) != std::string::npos) {
        std::string token = jobPath.substr(pos, sepPos - pos);
        if (!token.empty()) {
            tokens.push_back(token);
        }
        pos = sepPos + 1;
    }
    {
        std::string token = jobPath.substr(pos, sepPos - pos);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    if (tokens.empty()) {
        return root;
    }
    else {
        while (tokens.size() > 1) {
            auto taskName = tokens.front();
            tokens.pop_front();
            root = root->findChild<JobConfig*>((taskName.empty() ? QString() : QString(taskName.c_str())));
            if (!root) {
                return nullptr;
            }
        }
        jobName = tokens.front();
    }
    return root;
}

JobConfig* JobConfig::getJobConfig(const std::string& jobPath) const {
    std::string jobName;
    auto root = getRootConfig(jobPath, jobName);

    if (!root) {
        return nullptr;
    } 
    if (jobName.empty()) {
        return root;
    } else {

        auto found = root->findChild<JobConfig*>((jobName.empty() ? QString() : QString(jobName.c_str())));
        if (!found) {
            return nullptr;
        }
        return found;
    }
}

void SwitchConfig::setBranch(uint8_t branch) {
    if (_branch != branch) {
        _branch = branch;
        // We can re-use this signal here
        emit dirtyEnabled();
    }
}