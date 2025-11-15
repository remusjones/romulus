//
// Created by Remus on 22/12/2023.
//

#include "Entity.h"
#include <vector>
#include "Components/Component.h"


void Entity::Construct()
{
	for (const auto& component : componentMap)
	{
		component.second->Initialize();
	}
}

void Entity::Tick(const float aDeltaTime)
{
	for (const auto& component : componentMap)
	{
		component.second->Tick(aDeltaTime);
	}
}

void Entity::Cleanup()
{
	// todo: this is strange
	const std::vector<std::pair<std::string, Component*>> copy(componentMap.begin(), componentMap.end());
	for (const auto& component : copy)
	{
		component.second->Destroy();
		delete component.second;
	}
}

void Entity::AddComponent(Component* component)
{
	component->SetEntity(this);
	componentMap[component->GetName()] = component;
}

void Entity::RemoveComponent(Component* component)
{
	if (const auto it = componentMap.find(component->GetName()); it != componentMap.end())
	{
		componentMap.erase(it);
	}
}

void Entity::OnImGuiRender()
{
	transform.OnImGuiRender();
}
