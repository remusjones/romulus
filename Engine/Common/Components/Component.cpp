//
// Created by Remus on 13/01/2024.
//


#include "Component.h"
#include "Objects/Entity.h"

void Component::SetEntity(Entity* entity)
{
	attachedEntity = entity;
}

void Component::ClearEntity()
{
	attachedEntity = nullptr;
}

void Component::SetName(const std::string& inName)
{
	name = inName;
}

std::string& Component::GetName()
{
	return name;
}

void Component::Tick(float deltaTime)
{
}

void Component::Destroy()
{
	attachedEntity->RemoveComponent(this);
}

void Component::Initialize()
{
}
