//
//  SpaceClassifier.h
//  libraries/workload/src/workload
//
//  Created by Andrew Meadows 2018.02.21
//  Copyright 2018 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#ifndef hifi_workload_SpaceClassifier_h
#define hifi_workload_SpaceClassifier_h

#include "ViewTask.h"
#include "RegionTracker.h"

namespace workload {
class PerformSpaceTransactionConfig : public Job::Config {
    Q_OBJECT
public:
signals:
    void dirty();

protected:
};

class PerformSpaceTransaction {
public:
    using Config = PerformSpaceTransactionConfig;
    using JobModel = Job::Model<PerformSpaceTransaction, Config>;

    void configure(const Config& config);
    void run(const WorkloadContextPointer& context);

protected:
};

class SpaceClassifierTask {
public:
    using Inputs = Views;
    using Outputs = RegionTracker::Outputs;
    using JobModel = Task::ModelIO<SpaceClassifierTask, Inputs, Outputs>;
    void build(JobModel& model, const Varying& in, Varying& out);
};

class UpdatePhaseConfig : public Job::Config {
    Q_OBJECT
    Q_PROPERTY(glm::vec3 loadingOrigin READ getLoadingOrigin NOTIFY newStats())
    Q_PROPERTY(QVector3D loadingOriginQ READ getLoadingOriginQ NOTIFY newStats())
    Q_PROPERTY(float loadingRadius READ getLoadingRadius NOTIFY newStats())
    Q_PROPERTY(float readyRadius READ getReadyRadius NOTIFY newStats())
    Q_PROPERTY(int numEvaluated READ getNumEvaluated NOTIFY newStats())

public:
    glm::vec3 _loadingOrigin;
    float _loadingRadius{ 0.0f };
    float _readyRadius{ 0.0f };
    int _numEvaluated{ 0 };

    glm::vec3
    getLoadingOrigin() const {
        return _loadingOrigin;
    }
    QVector3D getLoadingOriginQ() const { return QVector3D(_loadingOrigin.x, _loadingOrigin.y, _loadingOrigin.z); }

    float getLoadingRadius() const { return _loadingRadius; }
    float getReadyRadius() const { return _readyRadius; }
    int getNumEvaluated() const { return _numEvaluated; }

signals:
    void dirty();

private:
};

class UpdatePhase {
public:
    using Inputs = Views;
    using Outputs = float;
    using Config = UpdatePhaseConfig;
    using JobModel = Job::ModelIO<UpdatePhase, Inputs, Outputs, Config>;

    void configure(const Config& config);
    void run(const WorkloadContextPointer& context, const Inputs& in, Outputs& out);

protected:
    glm::vec3 _loadingOrigin{ 0.0f };
    float _loadingRadius{ 0.0f };
    float _readyRadius{ 0.0f };
};

}  // namespace workload

#endif  // hifi_workload_SpaceClassifier_h
