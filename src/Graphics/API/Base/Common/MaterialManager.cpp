#include "MaterialManager.h"

void MaterialManager::AddMaterial(const std::shared_ptr<Material>& inMaterial)
{
	materials.push_back(inMaterial);
}

Material* MaterialManager::MakeMaterials() const
{
	for (const auto& material : materials)
	{
		material->Create(material.get());
	}

	// todo: this can crash
	return materials[0].get();
}
