//
// Created by Remus on 13/01/2024.
//

#pragma once
#include <string>

#include "EASTL/string.h"


class Entity;

class Component
{
public:
	virtual ~Component() = default;

	virtual void SetEntity(Entity* entity);
	virtual void ClearEntity();

	virtual void SetName(const eastl::string& inName);
	virtual eastl::string& GetName();

	virtual void Tick(float deltaTime);
	virtual void Destroy();
	virtual void Initialize();

protected:
	Entity* attachedEntity = nullptr;

	// todo: Replace with Hash
	eastl::string name;
};
