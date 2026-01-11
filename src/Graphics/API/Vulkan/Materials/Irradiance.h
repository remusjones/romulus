//
// Created by Remus on 10/01/2024.
//

#pragma once
#include "Base/Common/Material.h"


class Irradiance : Material
{
public:
    explicit Irradiance(const eastl::string_view& materialName)
        : Material(materialName)
    {
    }

    void Create() override;
    void Destroy() override;
};
