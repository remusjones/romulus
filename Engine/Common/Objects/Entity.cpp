//
// Created by Remus on 22/12/2023.
//

#include "Entity.h"
#include <vector>
#include "Components/Component.h"
#include "EASTL/vector.h"


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
	for (const auto& component : componentMap)
	{
		component.second->Destroy();

	}
	componentMap.clear();
}

void Entity::AddComponent(eastl::unique_ptr<Component> component)
{
	component->SetEntity(this);
	componentMap.insert_or_assign(component->GetName(), eastl::move(component));
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
