//
// Created by Remus on 24/12/2023.
//

#pragma once
#include "EASTL/hash_map.h"
#include "EASTL/string.h"

// todo: this should probably just be a mixin
class ImGuiLayer
{
public:
	virtual ~ImGuiLayer() = default;

	ImGuiLayer();
	virtual void OnImGuiRender() = 0;
	const char* GetUniqueLabel(const char* label); // Todo: Refactor method of generating unique labels (Urgently)


private:
	/**
	 * \brief
	 * Not Guaranteed to be Unique, but good enough for this use-case for now
	 * https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library
	 * \return
	 * Unique String
	 */
	static eastl::string MakeGuid();
	eastl::hash_map<const char*, eastl::string> constructedLabels;
};
