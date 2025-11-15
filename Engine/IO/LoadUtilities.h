//
// Created by Remus on 27/12/2023.
//

#pragma once
#include <string>
#include "VulkanGraphicsImpl.h"


class Mesh;
class AllocatedImage;

struct Color_RGBA
{
	Color_RGBA(
		const unsigned char r,
		const unsigned char g,
		const unsigned char b,
		const unsigned char a) :
		R(r), G(g), B(b), A(a)
	{
	}

	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;
};

class LoadUtilities
{
public:
	static bool LoadImageFromDisk(const VulkanGraphics* aEngine, const char* aFilePath, AllocatedImage& aResult);
	static bool LoadImagesFromDisk(const VulkanGraphics* engine, const std::vector<std::string>& aPaths,
	                               AllocatedImage& aResult, VkImageCreateFlags
	                               aImageCreateFlags = 0);

	static bool CreateImage(int aWidth, int aHeight, VulkanGraphics* aEngine, AllocatedImage& aResult,
	                        Color_RGBA aColor);
	static bool LoadMeshFromDisk(const char* filePath,
	                             std::vector<Vertex>& resultVertices,
	                             std::vector<int32_t>& resultIndices);
};
