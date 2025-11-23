//
// Created by Remus on 3/02/2024.
//

#pragma once
#include "IDebuggable.h"
#include "EASTL/hash_set.h"

class Editor : public IDebuggable
{
public:
	void OnDebugGui() override;
};

class IDebugRegistry
{
public:
	virtual ~IDebugRegistry() = default;
	virtual void Register(IDebuggable* object) = 0;
	virtual void Unregister(IDebuggable* object) = 0;
};

class DebugManager final : public  IDebugRegistry
{
public:
	void Register(IDebuggable* object) override;
	void Unregister(IDebuggable* object) override;
	void DrawImGui();

private:
	eastl::hash_set<IDebuggable*> debugLayersToDraw;
};
