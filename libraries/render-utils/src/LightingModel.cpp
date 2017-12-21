//
//  LightingModel.cpp
//  libraries/render-utils/src/
//
//  Created by Sam Gateau 7/1/2016.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//
#include "LightingModel.h"

void LightingModel::resetParameters(LightingModelParameters& params) {

    params.enableUnlit = 1.0f;
    params.enableEmissive = 1.0f;
    params.enableLightmap = 1.0f;
    params.enableBackground = 1.0f;

    params.enableScattering = 1.0f;
    params.enableDiffuse = 1.0f;
    params.enableSpecular = 1.0f;
    params.enableAlbedo = 1.0f;

    params.enableAmbientLight = 1.0f;
    params.enableDirectionalLight = 1.0f;
    params.enablePointLight = 1.0f;
    params.enableSpotLight = 1.0f;

    params.showLightContour = 0.0f; // false by default
    params.enableObscurance = 1.0f;
    params.enableMaterialTexturing = 1.0f;
    params.enableWireframe = 0.0f; // false by default

    params.showTexcoord = 0.0f; // false by default
    params.majorGrid = 1.0f;
    params.showDecalTexcoord = 0.0f;  // false by default
    params.enableDecal = 1.0f;

    params.decalRect = glm::vec4(0.0f);

    params.decalAlpha = 1.0;
    params.decalRotation = 0.0;
    params.decalRotation = 0.0;
    params.decalRotation = 0.0;

}

LightingModel::LightingModel() {
    _parametersBuffer = gpu::StructBuffer<LightingModelParameters>();
    resetParameters(_parametersBuffer.edit());
}

void LightingModel::setUnlit(bool enable) {
    if (enable != isUnlitEnabled()) {
        _parametersBuffer.edit().enableUnlit = (float) enable;
    }
}
bool LightingModel::isUnlitEnabled() const {
    return (bool)_parametersBuffer.get().enableUnlit;
}

void LightingModel::setEmissive(bool enable) {
    if (enable != isEmissiveEnabled()) {
        _parametersBuffer.edit().enableEmissive = (float)enable;
    }
}
bool LightingModel::isEmissiveEnabled() const {
    return (bool)_parametersBuffer.get().enableEmissive;
}
void LightingModel::setLightmap(bool enable) {
    if (enable != isLightmapEnabled()) {
        _parametersBuffer.edit().enableLightmap = (float)enable;
    }
}
bool LightingModel::isLightmapEnabled() const {
    return (bool)_parametersBuffer.get().enableLightmap;
}

void LightingModel::setBackground(bool enable) {
    if (enable != isBackgroundEnabled()) {
        _parametersBuffer.edit().enableBackground = (float)enable;
    }
}
bool LightingModel::isBackgroundEnabled() const {
    return (bool)_parametersBuffer.get().enableBackground;
}
void LightingModel::setObscurance(bool enable) {
    if (enable != isObscuranceEnabled()) {
        _parametersBuffer.edit().enableObscurance = (float)enable;
    }
}
bool LightingModel::isObscuranceEnabled() const {
    return (bool)_parametersBuffer.get().enableObscurance;
}

void LightingModel::setScattering(bool enable) {
    if (enable != isScatteringEnabled()) {
        _parametersBuffer.edit().enableScattering = (float)enable;
    }
}
bool LightingModel::isScatteringEnabled() const {
    return (bool)_parametersBuffer.get().enableScattering;
}

void LightingModel::setDiffuse(bool enable) {
    if (enable != isDiffuseEnabled()) {
        _parametersBuffer.edit().enableDiffuse = (float)enable;
    }
}
bool LightingModel::isDiffuseEnabled() const {
    return (bool)_parametersBuffer.get().enableDiffuse;
}
void LightingModel::setSpecular(bool enable) {
    if (enable != isSpecularEnabled()) {
        _parametersBuffer.edit().enableSpecular = (float)enable;
    }
}
bool LightingModel::isSpecularEnabled() const {
    return (bool)_parametersBuffer.get().enableSpecular;
}
void LightingModel::setAlbedo(bool enable) {
    if (enable != isAlbedoEnabled()) {
        _parametersBuffer.edit().enableAlbedo = (float)enable;
    }
}
bool LightingModel::isAlbedoEnabled() const {
    return (bool)_parametersBuffer.get().enableAlbedo;
}

void LightingModel::setMaterialTexturing(bool enable) {
    if (enable != isMaterialTexturingEnabled()) {
        _parametersBuffer.edit().enableMaterialTexturing = (float)enable;
    }
}
bool LightingModel::isMaterialTexturingEnabled() const {
    return (bool)_parametersBuffer.get().enableMaterialTexturing;
}

void LightingModel::setAmbientLight(bool enable) {
    if (enable != isAmbientLightEnabled()) {
        _parametersBuffer.edit().enableAmbientLight = (float)enable;
    }
}
bool LightingModel::isAmbientLightEnabled() const {
    return (bool)_parametersBuffer.get().enableAmbientLight;
}
void LightingModel::setDirectionalLight(bool enable) {
    if (enable != isDirectionalLightEnabled()) {
        _parametersBuffer.edit().enableDirectionalLight = (float)enable;
    }
}
bool LightingModel::isDirectionalLightEnabled() const {
    return (bool)_parametersBuffer.get().enableDirectionalLight;
}
void LightingModel::setPointLight(bool enable) {
    if (enable != isPointLightEnabled()) {
        _parametersBuffer.edit().enablePointLight = (float)enable;
    }
}
bool LightingModel::isPointLightEnabled() const {
    return (bool)_parametersBuffer.get().enablePointLight;
}
void LightingModel::setSpotLight(bool enable) {
    if (enable != isSpotLightEnabled()) {
        _parametersBuffer.edit().enableSpotLight = (float)enable;
    }
}
bool LightingModel::isSpotLightEnabled() const {
    return (bool)_parametersBuffer.get().enableSpotLight;
}

void LightingModel::setShowLightContour(bool enable) {
    if (enable != isShowLightContourEnabled()) {
        _parametersBuffer.edit().showLightContour = (float)enable;
    }
}
bool LightingModel::isShowLightContourEnabled() const {
    return (bool)_parametersBuffer.get().showLightContour;
}

void LightingModel::setWireframe(bool enable) {
    if (enable != isWireframeEnabled()) {
        _parametersBuffer.edit().enableWireframe = (float)enable;
    }
}
bool LightingModel::isWireframeEnabled() const {
    return (bool)_parametersBuffer.get().enableWireframe;
}

void LightingModel::setShowTexcoord(bool enable) {
    if (enable != isShowTexcoordEnabled()) {
        _parametersBuffer.edit().showTexcoord = (float)enable;
    }
}
bool LightingModel::isShowTexcoordEnabled() const {
    return (bool)_parametersBuffer.get().showTexcoord;
}

void LightingModel::setTexcoordMajorGrid(float majorGrid) {
    if (majorGrid != getTexcoordMajorGrid()) {
        _parametersBuffer.edit().majorGrid = (float)majorGrid;
    }
}
float LightingModel::getTexcoordMajorGrid() const {
    return (float)_parametersBuffer.get().majorGrid;
}

void LightingModel::setShowDecalTexcoord(bool enable) {
    if (enable != isShowDecalTexcoordEnabled()) {
        _parametersBuffer.edit().showDecalTexcoord = (float)enable;
    }
}
bool LightingModel::isShowDecalTexcoordEnabled() const {
    return (bool)_parametersBuffer.get().showDecalTexcoord;
}

void LightingModel::setDecal(bool enable) {
    if (enable != isDecalEnabled()) {
        _parametersBuffer.edit().enableDecal = (float)enable;
    }
}
bool LightingModel::isDecalEnabled() const {
    return (bool)_parametersBuffer.get().enableDecal;
}

void LightingModel::setDecalRect(const glm::vec4& rect) {
    if (rect != getDecalRect()) {
        _parametersBuffer.edit().decalRect = rect;
    }
}
glm::vec4 LightingModel::getDecalRect() const {
    return _parametersBuffer.get().decalRect;
}

void LightingModel::setDecalAlpha(float alpha) {
    if (alpha != getDecalAlpha()) {
        _parametersBuffer.edit().decalAlpha = alpha;
    }
}
float LightingModel::getDecalAlpha() const {
    return _parametersBuffer.get().decalAlpha;
}

void LightingModel::setDecalRotation(float rotation) {
    if (rotation != getDecalRotation()) {
        _parametersBuffer.edit().decalRotation = rotation;
        _parametersBuffer.edit().decalRotCos = cosf(rotation);
        _parametersBuffer.edit().decalRotSin = sinf(rotation);
    }
}
float LightingModel::getDecalRotation() const {
    return _parametersBuffer.get().decalRotation;
}

MakeLightingModel::MakeLightingModel() {
    _lightingModel = std::make_shared<LightingModel>();
}

void MakeLightingModel::configure(const Config& config) {
    _lightingModel->setUnlit(config.enableUnlit);
    _lightingModel->setEmissive(config.enableEmissive);
    _lightingModel->setLightmap(config.enableLightmap);
    _lightingModel->setBackground(config.enableBackground);

    _lightingModel->setObscurance(config.enableObscurance);

    _lightingModel->setScattering(config.enableScattering);
    _lightingModel->setDiffuse(config.enableDiffuse);
    _lightingModel->setSpecular(config.enableSpecular);
    _lightingModel->setAlbedo(config.enableAlbedo);

    _lightingModel->setMaterialTexturing(config.enableMaterialTexturing);

    _lightingModel->setAmbientLight(config.enableAmbientLight);
    _lightingModel->setDirectionalLight(config.enableDirectionalLight);
    _lightingModel->setPointLight(config.enablePointLight);
    _lightingModel->setSpotLight(config.enableSpotLight);

    _lightingModel->setShowLightContour(config.showLightContour);
    _lightingModel->setWireframe(config.enableWireframe);

    _lightingModel->setShowTexcoord(config.showTexcoord);
    _lightingModel->setTexcoordMajorGrid(config.majorGrid);

    _lightingModel->setShowDecalTexcoord(config.showDecalTexcoord);

    _lightingModel->setDecal(config.enableDecal);
    _lightingModel->setDecalRect(glm::vec4(config.decalRectTX, config.decalRectTY, config.decalRectSX * config.decalRectScale, config.decalRectSY * config.decalRectScale));
    _lightingModel->setDecalAlpha(config.decalAlpha);
    _lightingModel->setDecalRotation(glm::radians(config.decalRotation));

}

void MakeLightingModel::run(const render::RenderContextPointer& renderContext, LightingModelPointer& lightingModel) {

    lightingModel = _lightingModel;

    // make sure the enableTexturing flag of the render ARgs is in sync
    renderContext->args->_enableTexturing = _lightingModel->isMaterialTexturingEnabled();
}