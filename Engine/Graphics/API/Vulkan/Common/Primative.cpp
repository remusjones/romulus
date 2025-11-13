//
// Created by Remus on 9/01/2024.
//

#include "Primative.h"

void Primitive::Cleanup()
{
	Entity::Cleanup();
	if (renderer)
	{
		renderer->DestroyRenderer();
		delete renderer;
	}
}
