//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "ImGuiLayer.h"
#include "Math/Transform.h"
class Component;

// todo: Should this be called an entity?
// Ideally we use an actual entity system and this will get confusing!
class Entity : public ImGuiLayer
{
public:
	~Entity() override = default;

	virtual void Construct();

	virtual void Tick(float aDeltaTime);
	virtual void Cleanup();
	virtual void AddComponent(Component* aComponent);
	virtual void RemoveComponent(Component* aComponent);


	template <typename T>
	bool GetComponent(T& result)
	{
		for (auto& component : componentMap)
		{
			if (dynamic_cast<T*>(component.second))
			{
				result = *static_cast<T*>(component.second);
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
	bool GetComponent(const std::string& name, T& result)
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

	std::unordered_map<std::string, Component*> componentMap;
};
