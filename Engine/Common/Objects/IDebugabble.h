//
// Created by Remus on 24/12/2023.
//

#pragma once
#include "EASTL/hash_map.h"
#include "EASTL/string.h"

// todo: this should probably just be a mixin
class IDebugabble
{
public:
	virtual ~IDebugabble() = default;
	virtual void OnDebugGui() = 0;
};