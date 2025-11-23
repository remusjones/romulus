#pragma once
#include <memory>

#include "Base/Common/Material.h"


class DefaultMaterial : public Material {
public:

    enum BindingLocation {
        SCENE_INFORMATION = 0,
        PROPERTIES = 1,
        TEXTURE = 2
    };
    enum TextureComposite {
        ALBEDO = 0,
        NORMAL = 1
    };


    DefaultMaterial(const eastl::string_view& inMaterialName);
    DefaultMaterial() = default;


    void Create(MaterialBase *baseMaterial = nullptr) override;

    void OnDebugGui() override;

    void MakeDefaults() const;

    void Destroy() override;

    std::unique_ptr<Texture> mTextures;

    std::vector<const char *> debugColors{
        "Default",
        "normal",
        "lightDirection",
        "viewDirection",
        "reflectionDirection",
        "ambientColor",
        "diffuseColor",
        "specularColor",
        "texColor",
        "inUV",
    };

    static constexpr const char* DefaultAssetPaths[] = {
        "/Assets/Textures/Defaults/albedo_4096.png",
        "/Assets/Textures/Defaults/normal_4096.png",
    };
};
