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
	template<typename T>
	T& Create(eastl::string_view materialName)
	{
		static_assert(eastl::is_base_of_v<Material, T>, "T must derive from Material");
		eastl::unique_ptr<T> material = eastl::make_unique<T>(materialName);
		T* materialPtr = material.get();
		materialPtr->Create();
		materials.push_back(eastl::move(material));

		return *materialPtr;
	}

	void Destroy()
	{
		for (const auto& material : materials)
		{
			material->Destroy();
		}
		materials.clear();
	}

	[[nodiscard]] eastl::vector<VkDescriptorSetLayout> GetDescriptorLayouts() const
	{
		eastl::vector<VkDescriptorSetLayout> descriptors(materials.size());

		for (int i = 0; i < materials.size(); i++)
		{
			descriptors[i] = materials[i]->GetDescriptorLayout();
		}
		return descriptors;
	}

	eastl::vector<eastl::unique_ptr<Material>> materials = {};
};
