//
// Created by Remus on 22/12/2023.
//

#include "SceneObject.h"

#include "imgui_internal.h"
#include "Components/Component.h"

void SceneObject::Construct()
{
	for (const auto& component : components)
	{
		component.second->Initialize();
	}
}

void SceneObject::Tick(const float aDeltaTime)
{
	for (const auto& component : components)
	{
		component.second->Tick(aDeltaTime);
	}
}

void SceneObject::Destroy()
{
	// todo: this is strange
	for (const auto& component : components)
	{
		component.second->Destroy();

	}
	components.clear();
}

void SceneObject::AddComponent(eastl::unique_ptr<Component> component)
{
	component->SetEntity(this);
	components.insert_or_assign(component->GetName(), eastl::move(component));
}

void SceneObject::RemoveComponent(Component* component)
{
	if (const auto it = components.find(component->GetName()); it != components.end())
	{
		components.erase(it);
	}
}

void SceneObject::OnDebugGui()
{
	ImGui::PushID(&transform);
	transform.OnDebugGui();
	ImGui::PopID();
}
