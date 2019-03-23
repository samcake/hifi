//
//  MaterialBaker.h
//  libraries/baking/src
//
//  Created by Sam Gondelman on 2/26/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_MaterialBaker_h
#define hifi_MaterialBaker_h

#include "Baker.h"

#include "TextureBaker.h"
#include "baking/TextureFileNamer.h"

#include <material-networking/MaterialCache.h>

static const QString BAKED_MATERIAL_EXTENSION = ".baked.json";

class MaterialBaker : public Baker {
    Q_OBJECT
public:
    MaterialBaker(const QString& materialData, bool isURL, const QString& bakedOutputDir, const QUrl& destinationPath);

    QString getMaterialData() const { return _materialData; }
    bool isURL() const { return _isURL; }
    QString getBakedMaterialData() const { return _bakedMaterialData; }

    static void setNextOvenWorkerThreadOperator(std::function<QThread*()> getNextOvenWorkerThreadOperator) { _getNextOvenWorkerThreadOperator = getNextOvenWorkerThreadOperator; }

public slots:
    virtual void bake() override;

signals:
    void originalMaterialLoaded();

private slots:
    void processMaterial();
    void outputMaterial();
    void handleFinishedTextureBaker();

private:
    void loadMaterial();

    QString _materialData;
    bool _isURL;

    NetworkMaterialResourcePointer _materialResource;

    QHash<QPair<QUrl, image::TextureUsage::Type>, QSharedPointer<TextureBaker>> _textureBakers;
    QMultiHash<QPair<QUrl, image::TextureUsage::Type>, std::shared_ptr<NetworkMaterial>> _materialsNeedingRewrite;

    QString _bakedOutputDir;
    QString _textureOutputDir;
    QString _bakedMaterialData;
    QUrl _destinationPath;

    QScriptEngine _scriptEngine;
    static std::function<QThread*()> _getNextOvenWorkerThreadOperator;
    TextureFileNamer _textureFileNamer;
};

#endif // !hifi_MaterialBaker_h
