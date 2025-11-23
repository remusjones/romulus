//
// Created by Remus on 3/02/2024.
//

#pragma once
#include "IDebugabble.h"
#include "EASTL/vector.h"
#include "EASTL/weak_ptr.h"

class Editor : public IDebugabble
{
public:
	void OnDebugGui() override;
};

class IDebugRegistry
{
public:
	virtual ~IDebugRegistry() = default;
	virtual void Register(IDebugabble* object) = 0;
};

class DebugManager final : public  IDebugRegistry
{
public:
	void Register(IDebugabble* object) override;
	void DrawImGui();

private:
	eastl::vector<IDebugabble*> debugLayersToDraw;
};
