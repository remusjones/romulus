//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "IDebugabble.h"
#include "Components/Component.h"
#include "EASTL/unique_ptr.h"
#include "Math/Transform.h"

// todo: Should this be called an entity?
// Ideally we use an actual entity system and this will get confusing!
class SceneObject : public IDebugabble
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
		for (auto& component : components)
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
	bool GetComponents(eastl::vector<T>& result)
	{
		for (auto& component : components)
		{
			if (dynamic_cast<T*>(component.second))
			{
				result.push_back(*static_cast<T*>(component.second));
			}
		}
		return !result.empty();
	}

	template <class T>
	bool GetComponent(const eastl::string_view& inName, T& result)
	{
		if (components.contains(inName))
		{
			result = *static_cast<T*>(components[inName]);
			return true;
		}
		return false;
	}


	void OnDebugGui() override;

	Transform transform;
	eastl::string_view name;
	eastl::hash_map<eastl::string_view, eastl::unique_ptr<Component>> components;
};
