#pragma once
#include <memory>
#include <vector>

#include "Material.h"

class Material;

class MaterialManager
{
public:
	void AddMaterial(const std::shared_ptr<Material>& inMaterial);
	Material* MakeMaterials() const;

	// todo: evaluate usage of shared ptr here
	std::vector<std::shared_ptr<Material>> materials;
};
