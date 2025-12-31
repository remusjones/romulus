//
// Created by Remus on 9/01/2024.
//

#pragma once
#include <memory>

#include "Base/Common/Material.h"


class MaterialFactory
{
public:
	// todo: these material names are only here for debug
	// - we should probably strip these
	template <typename T>
	T& Create(const char* materialName = "Default")
	{
		static_assert(std::is_base_of_v<Material, T>, "T must derive from Material");
		std::unique_ptr<T> material = std::make_unique<T>(materialName);
		T* materialPtr = material.get();
		materials.push_back(std::move(material)); // move into array
		return *materialPtr;
	}

	void Make() const
	{
		for (const auto& material : materials)
		{
			material->Create(material.get());
		}
	}

	void Destroy()
	{
		for (const auto& material : materials)
		{
			material->Destroy();
		}
		materials.clear();
	}

	[[nodiscard]] std::vector<VkDescriptorSetLayout> GetDescriptorLayouts() const
	{
		std::vector<VkDescriptorSetLayout> descriptors(materials.size());

		for (int i = 0; i < materials.size(); i++)
			descriptors[i] = materials[i]->GetDescriptorLayout();

		return descriptors;
	}

	std::vector<std::unique_ptr<Material>> materials = {};
};
