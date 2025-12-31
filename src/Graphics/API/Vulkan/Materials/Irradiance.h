//
// Created by Remus on 10/01/2024.
//

#pragma once
#include "Base/Common/Material.h"


class Irradiance : Material {
public:
    void Create(MaterialBase *aBaseMaterial) override;
    void Destroy() override;
};
