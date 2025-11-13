//
// Created by Remus on 24/12/2023.
//

#include "ImGuiLayer.h"

#include <ios>
#include <random>
#include <sstream>

#include <Logger.h>


ImGuiLayer::ImGuiLayer() = default;

const char* ImGuiLayer::GetUniqueLabel(const char* aLabel)
{
	if (mConstructedLabels.contains(aLabel))
	{
		return mConstructedLabels[aLabel].c_str();
	}

	mConstructedLabels[aLabel] = std::string(aLabel + std::string("##") + MakeGuid());
	return mConstructedLabels[aLabel].c_str();
}

// Psuedo GUID implementation
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(0, 15);
static std::uniform_int_distribution<> dis2(8, 11);

std::string ImGuiLayer::MakeGuid()
{
	std::stringstream ss;
	int i;
	ss << std::hex;
	for (i = 0; i < 8; i++)
	{
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 4; i++)
	{
		ss << dis(gen);
	}
	ss << "-4";
	for (i = 0; i < 3; i++)
	{
		ss << dis(gen);
	}
	ss << "-";
	ss << dis2(gen);
	for (i = 0; i < 3; i++)
	{
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 12; i++)
	{
		ss << dis(gen);
	};
	return ss.str();
}
