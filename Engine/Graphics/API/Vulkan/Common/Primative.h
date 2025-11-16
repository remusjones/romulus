//
// Created by Remus on 9/01/2024.
//

#pragma once
#include "Objects/SceneObject.h"
#include "Vulkan/Renderers/Renderer.h"

/* Represents a position and a undefined renderer component */
class Primitive : public SceneObject
{
public:
	Primitive() = default;
	Primitive(const eastl::string_view& aEntityName) : renderer(nullptr) { name = aEntityName; }

	void Cleanup() override;

	// todo: make a getter, and protect
	Renderer* renderer;
};
