//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Objects/Entity.h"
#include "Vulkan/Renderers/Renderer.h"

/* Represents a position and a undefined renderer component */
class Primitive : public Entity
{
public:
	Primitive() = default;
	Primitive(const char* aEntityName) : renderer(nullptr) { mName = aEntityName; }

	void Cleanup() override;

	// todo: make a getter, and protect
	Renderer* renderer;
};
