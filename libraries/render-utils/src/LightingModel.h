//
//  LightingModel.h
//  libraries/render-utils/src/
//
//  Created by Sam Gateau 7/1/2016.
//  Copyright 2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_LightingModel_h
#define hifi_LightingModel_h

#include <gpu/Resource.h>

#include <render/Forward.h>
#include <render/DrawTask.h>

// LightingModel is  a helper class gathering in one place the flags to enable the lighting contributions
class LightingModel {
public:
    using UniformBufferView = gpu::BufferView;

    LightingModel();


    void setUnlit(bool enable);
    bool isUnlitEnabled() const;

    void setEmissive(bool enable);
    bool isEmissiveEnabled() const;
    void setLightmap(bool enable);
    bool isLightmapEnabled() const;

    void setBackground(bool enable);
    bool isBackgroundEnabled() const;

    void setObscurance(bool enable);
    bool isObscuranceEnabled() const;

    void setScattering(bool enable);
    bool isScatteringEnabled() const;
    void setDiffuse(bool enable);
    bool isDiffuseEnabled() const;
    void setSpecular(bool enable);
    bool isSpecularEnabled() const;

    void setAlbedo(bool enable);
    bool isAlbedoEnabled() const;

    void setMaterialTexturing(bool enable);
    bool isMaterialTexturingEnabled() const;

    void setAmbientLight(bool enable);
    bool isAmbientLightEnabled() const;
    void setDirectionalLight(bool enable);
    bool isDirectionalLightEnabled() const;
    void setPointLight(bool enable);
    bool isPointLightEnabled() const;
    void setSpotLight(bool enable);
    bool isSpotLightEnabled() const;

    void setShowLightContour(bool enable);
    bool isShowLightContourEnabled() const;

    void setWireframe(bool enable);
    bool isWireframeEnabled() const;

    void setShowTexcoord(bool enable);
    bool isShowTexcoordEnabled() const;

    void setTexcoordMajorGrid(float scale);
    float getTexcoordMajorGrid() const;

    void setShowDecalTexcoord(bool enable);
    bool isShowDecalTexcoordEnabled() const;

    void setDecal(bool enable);
    bool isDecalEnabled() const;

    void setDecalRect(const glm::vec4& rect);
    glm::vec4 getDecalRect() const;

    void setDecalAlpha(float alpha);
    float LightingModel::getDecalAlpha() const;

    UniformBufferView getParametersBuffer() const { return _parametersBuffer; }

protected:

    // Include the LightingModelParameters declaration
#include "LightingModel_shared.slh"
    using LightingModelBuffer = gpu::StructBuffer<LightingModelParameters>;

    LightingModelBuffer _parametersBuffer;

    static void resetParameters(LightingModelParameters& params);
};

using LightingModelPointer = std::shared_ptr<LightingModel>;




class MakeLightingModelConfig : public render::Job::Config {
    Q_OBJECT

    Q_PROPERTY(bool enableUnlit MEMBER enableUnlit NOTIFY dirty)
    Q_PROPERTY(bool enableEmissive MEMBER enableEmissive NOTIFY dirty)
    Q_PROPERTY(bool enableLightmap MEMBER enableLightmap NOTIFY dirty)
    Q_PROPERTY(bool enableBackground MEMBER enableBackground NOTIFY dirty)
    Q_PROPERTY(bool enableObscurance MEMBER enableObscurance NOTIFY dirty)

    Q_PROPERTY(bool enableScattering MEMBER enableScattering NOTIFY dirty)
    Q_PROPERTY(bool enableDiffuse MEMBER enableDiffuse NOTIFY dirty)
    Q_PROPERTY(bool enableSpecular MEMBER enableSpecular NOTIFY dirty)

    Q_PROPERTY(bool enableAlbedo MEMBER enableAlbedo NOTIFY dirty)
    Q_PROPERTY(bool enableMaterialTexturing MEMBER enableMaterialTexturing NOTIFY dirty)

    Q_PROPERTY(bool enableAmbientLight MEMBER enableAmbientLight NOTIFY dirty)
    Q_PROPERTY(bool enableDirectionalLight MEMBER enableDirectionalLight NOTIFY dirty)
    Q_PROPERTY(bool enablePointLight MEMBER enablePointLight NOTIFY dirty)
    Q_PROPERTY(bool enableSpotLight MEMBER enableSpotLight NOTIFY dirty)

    Q_PROPERTY(bool showLightContour MEMBER showLightContour NOTIFY dirty)
    Q_PROPERTY(bool enableWireframe MEMBER enableWireframe NOTIFY dirty)

    Q_PROPERTY(bool showTexcoord MEMBER showTexcoord NOTIFY dirty)
    Q_PROPERTY(float majorGrid MEMBER majorGrid NOTIFY dirty)
    Q_PROPERTY(bool showDecalTexcoord MEMBER showDecalTexcoord NOTIFY dirty)

    Q_PROPERTY(bool enableDecal MEMBER enableDecal NOTIFY dirty)
    Q_PROPERTY(float decalRectTX MEMBER decalRectTX NOTIFY dirty)
    Q_PROPERTY(float decalRectTY MEMBER decalRectTY NOTIFY dirty)
    Q_PROPERTY(float decalRectSX MEMBER decalRectSX NOTIFY dirty)
    Q_PROPERTY(float decalRectSY MEMBER decalRectSY NOTIFY dirty)
    Q_PROPERTY(float decalRectScale MEMBER decalRectScale NOTIFY dirty)
    Q_PROPERTY(float decalAlpha MEMBER decalAlpha NOTIFY dirty)

public:
    MakeLightingModelConfig() : render::Job::Config() {} // Make Lighting Model is always on

    bool enableUnlit{ true };
    bool enableEmissive{ true };
    bool enableLightmap{ true };
    bool enableBackground{ true };
    bool enableObscurance{ true };

    bool enableScattering{ true };
    bool enableDiffuse{ true };
    bool enableSpecular{ true };

    bool enableAlbedo{ true };
    bool enableMaterialTexturing { true };

    bool enableAmbientLight{ true };
    bool enableDirectionalLight{ true };
    bool enablePointLight{ true };
    bool enableSpotLight{ true };

    bool showLightContour { false }; // false by default

    bool enableWireframe{ false }; // false by default

    bool showTexcoord{ false }; // false by default
    float majorGrid{ 1.0 }; // false by default
    bool showDecalTexcoord{ false }; // false by default

    bool  enableDecal{ true };
    float decalRectTX{ 0.0 };
    float decalRectTY{ 0.0 };
    float decalRectSX{ 0.0 };
    float decalRectSY{ 0.0 };
    float decalRectScale{ 1.0 };
    float decalAlpha{ 1.0 };

signals:
    void dirty();
};

class MakeLightingModel {
public:
    using Config = MakeLightingModelConfig;
    using JobModel = render::Job::ModelO<MakeLightingModel, LightingModelPointer, Config>;

    MakeLightingModel();

    void configure(const Config& config);
    void run(const render::RenderContextPointer& renderContext, LightingModelPointer& lightingModel);

private:
    LightingModelPointer _lightingModel;
};

#endif // hifi_SurfaceGeometryPass_h
