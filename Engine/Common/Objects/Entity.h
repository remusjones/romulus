//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "ImGuiLayer.h"
#include "Components/Component.h"
#include "EASTL/unique_ptr.h"
#include "Math/Transform.h"

// todo: Should this be called an entity?
// Ideally we use an actual entity system and this will get confusing!
class Entity : public ImGuiLayer
{
public:
	virtual void Construct();

	virtual void Tick(float aDeltaTime);
	virtual void Cleanup();
	virtual void AddComponent(eastl::unique_ptr<Component> component);
	virtual void RemoveComponent(Component* component);


	template <typename T>
	bool GetComponent(T& result)
	{
		for (auto& component : componentMap)
		{
			if (dynamic_cast<T*>(component.second.get()))
			{
				result = *static_cast<T*>(component.second.get());
				return true;
			}
		}
		return false;
	}

	template <typename T>
	bool GetComponents(std::vector<T>& result)
	{
		for (auto& component : componentMap)
		{
			if (dynamic_cast<T*>(component.second))
			{
				result.push_back(*static_cast<T*>(component.second));
			}
		}
		return !result.empty();
	}

	template <class T>
	bool GetComponent(const char* name, T& result)
	{
		if (componentMap.contains(name))
		{
			result = *static_cast<T*>(componentMap[name]); // add static_cast here
			return true;
		}
		return false;
	}

	template <class T>
	bool GetComponent(const eastl::string& name, T& result)
	{
		if (componentMap.contains(name))
		{
			result = *static_cast<T*>(componentMap[name]); // add static_cast here
			return true;
		}
		return false;
	}


	void OnImGuiRender() override;

	Transform transform;
	const char* mName{"Default"};

	eastl::hash_map<eastl::string, eastl::unique_ptr<Component>> componentMap;
};
