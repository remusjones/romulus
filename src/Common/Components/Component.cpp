//
// Created by Remus on 13/01/2024.
//


#include "Component.h"
#include "Objects/SceneObject.h"

void Component::SetEntity(SceneObject* entity)
{
	attachedEntity = entity;
}

void Component::ClearEntity()
{
	attachedEntity = nullptr;
}

void Component::SetName(const eastl::string& inName)
{
	name = inName;
}

eastl::string& Component::GetName()
{
	return name;
}

void Component::Tick(float deltaTime)
{
}

void Component::Destroy()
{
}

void Component::Initialize()
{
}
