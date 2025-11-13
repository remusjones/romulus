//
// Created by Remus on 27/12/2023.
//

#include "LoadUtilities.h"
#include "tiny_obj_loader.h"
#include "Base/Common/Data/Mesh.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize2.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Logger.h>
#include "FileManagement.h"
#include "Vulkan/Helpers/VulkanInitialization.h"

struct IndexComparator
{
	bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const
	{
		// define your order here. For example:
		if (a.vertex_index != b.vertex_index)
		{
			return a.vertex_index < b.vertex_index;
		}
		if (a.normal_index != b.normal_index)
		{
			return a.normal_index < b.normal_index;
		}
		return a.texcoord_index < b.texcoord_index;
	}
};

bool LoadUtilities::LoadImageFromDisk(const VulkanGraphics* aEngine, const char* aFilePath, AllocatedImage& aResult)
{
	int texWidth, texHeight, texChannels;

	if (!FileManagement::FileExists(aFilePath))
	{
		Logger::Log(spdlog::level::critical, (std::string("File Path does not exist: ") + aFilePath).c_str());
	}

	stbi_uc* pixels = stbi_load(aFilePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels)
	{
		Logger::Log(spdlog::level::err, (std::string("Could not load file ") + aFilePath).c_str());
		return false;
	}

	const void* pixel_ptr        = pixels;
	const VkDeviceSize imageSize = texWidth * texHeight * 4;

	constexpr VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;


	const AllocatedBuffer stagingBuffer = AllocatedBuffer(pixel_ptr, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	stbi_image_free(pixels);


	VkExtent3D imageExtent;
	imageExtent.width  = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth  = 1;

	const VkImageCreateInfo dimg_info = VulkanInitialization::CreateImageInfo(image_format, VK_IMAGE_USAGE_SAMPLED_BIT |
	                                                                          VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	                                                                          imageExtent);

	AllocatedImage newImage;
	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;

	//allocate and create the image
	vmaCreateImage(aEngine->allocator, &dimg_info, &dimg_allocinfo, &newImage.mImage, &newImage.mAllocation, nullptr);

	aEngine->vulkanEngine.SubmitBufferCommand([&](VkCommandBuffer cmd)
	{
		VkImageSubresourceRange range;
		range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel   = 0;
		range.levelCount     = 1;
		range.baseArrayLayer = 0;
		range.layerCount     = 1;

		VkImageMemoryBarrier imageBarrier_toTransfer = {};
		imageBarrier_toTransfer.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageBarrier_toTransfer.oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier_toTransfer.newLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toTransfer.image            = newImage.mImage;
		imageBarrier_toTransfer.subresourceRange = range;

		imageBarrier_toTransfer.srcAccessMask = 0;
		imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
		                     nullptr, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset      = 0;
		copyRegion.bufferRowLength   = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel       = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount     = 1;
		copyRegion.imageExtent                     = imageExtent;

		//copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, stagingBuffer.GetBuffer(), newImage.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
		                       &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

		imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
		                     0, nullptr, 1, &imageBarrier_toReadable);
	});
	vmaDestroyBuffer(aEngine->allocator, stagingBuffer.GetBuffer(), stagingBuffer.GetAllocation());
	aResult = newImage;
	return true;
}

bool LoadUtilities::LoadImagesFromDisk(const VulkanGraphics* aEngine, const std::vector<std::string>& aPaths,
                                       AllocatedImage& aResult, VkImageCreateFlags aImageCreateFlags)
{
	const int imageCount      = aPaths.size();
	constexpr VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
	// TODO: Fix ForcedFormat as its a workaround for mismatching types
	constexpr int forcedFormat = STBI_rgb_alpha; // Forces all loaded images to be a rgba for simplicityy

	VkDeviceSize layerSize = 0;
	VkDeviceSize totalSize = 0;

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels;

	int maxTexWidth  = 0;
	int maxTexHeight = 0;

	// Find maximum dimensions.
	for (const auto& path : aPaths)
	{
		if (!FileManagement::FileExists(path))
			Logger::Log(spdlog::level::critical, (std::string("File Path does not exist: ") + path).c_str());

		stbi_info(path.c_str(), &texWidth, &texHeight, &texChannels);
		maxTexWidth  = std::max(maxTexWidth, texWidth);
		maxTexHeight = std::max(maxTexHeight, texHeight);
		totalSize += texWidth * texHeight * forcedFormat;
	}
	//Create the staging buffer
	AllocatedBuffer stagingBuffer;
	stagingBuffer.Create(totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

	std::string bufferName;
	bufferName.append(aPaths[0]);
	vmaSetAllocationName(gGraphics->allocator, stagingBuffer.GetAllocation(), (bufferName + " staging buffer").c_str());

	// Map the memory
	void* data;
	vmaMapMemory(gGraphics->allocator, stagingBuffer.GetAllocation(), &data);
	uint64_t memAddress = reinterpret_cast<uint64_t>(data);

	for (int i = 0; i < imageCount; i++)
	{
		// Load the image file
		pixels    = stbi_load(aPaths[i].c_str(), &texWidth, &texHeight, &texChannels, forcedFormat);
		layerSize = texWidth * texHeight * forcedFormat;

		if (pixels)
		{
			// Check if image needs to be upscaled to largest image
			if (texWidth < maxTexWidth || texHeight < maxTexHeight)
			{
				Logger::Log(spdlog::level::debug, (std::string("Upscaling Image: ") + aPaths[i]).c_str());
				const float aspectRatio = static_cast<float>(texWidth) / static_cast<float>(texHeight);
				int newWidth            = texWidth;
				int newHeight           = texHeight;

				if (texWidth < maxTexWidth)
				{
					newWidth  = maxTexWidth;
					newHeight = static_cast<int>(newWidth / aspectRatio);
					if (newHeight > maxTexHeight)
					{
						newHeight = maxTexHeight;
						newWidth  = static_cast<int>(newHeight * aspectRatio);
					}
				}
				if (texHeight < maxTexHeight)
				{
					newHeight = maxTexHeight;
					newWidth  = static_cast<int>(newHeight * aspectRatio);
					if (newWidth > maxTexWidth)
					{
						newWidth  = maxTexWidth;
						newHeight = static_cast<int>(newWidth / aspectRatio);
					}
				}

				// Create a new array and resize the image
				uint8_t* newPixels = new uint8_t[newWidth * newHeight * texChannels];

				if (!stbir_resize_uint8_srgb(pixels, texWidth, texHeight, 0, newPixels, newWidth, newHeight, 0,
				                             STBIR_RGBA))
				{
					// Error during resize, you can use logger here
					Logger::Log(spdlog::level::err, (std::string("Failed to Resize Image: ") + aPaths[i]).c_str());
				}

				// Free the original image and copy the resized one
				stbi_image_free(pixels);
				pixels    = newPixels;
				layerSize = newWidth * newHeight * texChannels;
			}

			// Copy data to memory
			memcpy(reinterpret_cast<void*>(memAddress), pixels, layerSize);
			memAddress += layerSize;
			stbi_image_free(pixels);
		}
		else
		{
			Logger::Log(spdlog::level::err, (std::string("Could not load file ") + aPaths[i]).c_str());
		}
	}

	vmaUnmapMemory(gGraphics->allocator, stagingBuffer.GetAllocation());


	VkExtent3D imageExtent;
	imageExtent.width  = static_cast<uint32_t>(maxTexWidth);
	imageExtent.height = static_cast<uint32_t>(maxTexHeight);
	imageExtent.depth  = 1;

	VkImageCreateInfo dimg_info = {};
	dimg_info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	dimg_info.pNext             = nullptr;

	dimg_info.imageType = VK_IMAGE_TYPE_2D;

	dimg_info.format = format;
	dimg_info.extent = imageExtent;

	dimg_info.mipLevels   = 1;
	dimg_info.arrayLayers = imageCount;
	dimg_info.samples     = VK_SAMPLE_COUNT_1_BIT;
	dimg_info.tiling      = VK_IMAGE_TILING_OPTIMAL;
	dimg_info.usage       = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	dimg_info.flags       = aImageCreateFlags;

	AllocatedImage newImage;
	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;

	const VkResult result = vmaCreateImage(aEngine->allocator, &dimg_info, &dimg_allocinfo, &newImage.mImage,
	                                       &newImage.mAllocation, nullptr);


	vmaSetAllocationName(gGraphics->allocator, newImage.mAllocation,
	                     (bufferName + "  VkImage").c_str());

	if (result != VK_SUCCESS)
	{
		Logger::Log(spdlog::level::err, "vmaCreateImage failed!");
		vmaDestroyBuffer(gGraphics->allocator, stagingBuffer.GetBuffer(), stagingBuffer.GetAllocation());
		return false;
	}
	if (!newImage.mImage || !newImage.mAllocation)
	{
		Logger::Log(spdlog::level::err, "Image or allocation from vmaCreateImage is null!");
		vmaDestroyBuffer(gGraphics->allocator, stagingBuffer.GetBuffer(), stagingBuffer.GetAllocation());
		return false;
	}
	aEngine->vulkanEngine.SubmitBufferCommand([&](VkCommandBuffer cmd)
	{
		VkImageSubresourceRange range;
		range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel   = 0;
		range.levelCount     = 1;
		range.baseArrayLayer = 0;
		range.layerCount     = imageCount;

		VkImageMemoryBarrier imageBarrier_toTransfer = {};
		imageBarrier_toTransfer.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageBarrier_toTransfer.oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier_toTransfer.newLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toTransfer.image            = newImage.mImage;
		imageBarrier_toTransfer.subresourceRange = range;

		imageBarrier_toTransfer.srcAccessMask = 0;
		imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
		                     nullptr, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset      = 0;
		copyRegion.bufferRowLength   = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel       = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount     = imageCount;
		copyRegion.imageExtent                     = imageExtent;

		//copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, stagingBuffer.GetBuffer(), newImage.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
		                       &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

		imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
		                     0, nullptr, 1, &imageBarrier_toReadable);
	});
	// Cleanup staging buffer
	vmaDestroyBuffer(aEngine->allocator, stagingBuffer.GetBuffer(), stagingBuffer.GetAllocation());
	aResult = newImage;
	return true;
}

bool LoadUtilities::CreateImage(const int aWidth, const int aHeight,
                                VulkanGraphics* aEngine,
                                AllocatedImage& aResult,
                                Color_RGBA aColor = Color_RGBA(1, 1, 1, 1))
{
	const int texWidth  = aWidth;
	const int texHeight = aHeight; // Assuming RGB texture
	stbi_uc* pixels     = new stbi_uc[4 * texWidth * texHeight];
	for (int i = 0; i < 4 * texWidth * texHeight; i += 4)
	{
		pixels[i]     = aColor.R; // red
		pixels[i + 1] = aColor.G; // green
		pixels[i + 2] = aColor.B; // blue
		pixels[i + 3] = aColor.A; // alpha
	}

	const void* pixel_ptr        = pixels;
	const VkDeviceSize imageSize = texWidth * texHeight * 4;

	const VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;


	AllocatedBuffer stagingBuffer = AllocatedBuffer(pixel_ptr, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	delete[] pixels;

	VkExtent3D imageExtent;
	imageExtent.width  = static_cast<uint32_t>(texWidth);
	imageExtent.height = static_cast<uint32_t>(texHeight);
	imageExtent.depth  = 1;

	VkImageCreateInfo dimg_info = VulkanInitialization::CreateImageInfo(image_format, VK_IMAGE_USAGE_SAMPLED_BIT |
	                                                                    VK_IMAGE_USAGE_TRANSFER_DST_BIT,
	                                                                    imageExtent);

	AllocatedImage newImage;
	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;

	//allocate and create the image
	vmaCreateImage(aEngine->allocator, &dimg_info, &dimg_allocinfo, &newImage.mImage, &newImage.mAllocation, nullptr);

	aEngine->vulkanEngine.SubmitBufferCommand([&](VkCommandBuffer cmd)
	{
		VkImageSubresourceRange range;
		range.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel   = 0;
		range.levelCount     = 1;
		range.baseArrayLayer = 0;
		range.layerCount     = 1;

		VkImageMemoryBarrier imageBarrier_toTransfer = {};
		imageBarrier_toTransfer.sType                = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageBarrier_toTransfer.oldLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier_toTransfer.newLayout        = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toTransfer.image            = newImage.mImage;
		imageBarrier_toTransfer.subresourceRange = range;

		imageBarrier_toTransfer.srcAccessMask = 0;
		imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
		                     nullptr, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset      = 0;
		copyRegion.bufferRowLength   = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel       = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount     = 1;
		copyRegion.imageExtent                     = imageExtent;

		//copy the buffer into the image
		vkCmdCopyBufferToImage(cmd, stagingBuffer.GetBuffer(), newImage.mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
		                       &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

		imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
		                     0, nullptr, 1, &imageBarrier_toReadable);
	});
	stagingBuffer.Destroy();
	aResult = newImage;
	return true;
}


bool LoadUtilities::LoadMeshFromDisk(const char* filePath, std::vector<Vertex>& resultVertices,
                                     std::vector<int32_t>& resultIndices)
{
	//attrib will contain the vertex arrays of the file
	tinyobj::attrib_t attrib;
	//shapes contains the info for each separate object in the file
	std::vector<tinyobj::shape_t> shapes;
	//materials contains the information about the material of each shape, but we won't use it.
	std::vector<tinyobj::material_t> materials;

	//error and warning output from the load function
	std::string warn;
	std::string err;

	if (!FileManagement::FileExists(filePath))
	{
		Logger::Log(spdlog::level::critical, (std::string("File Path does not exist: ") + filePath).c_str());
	}

	//l oad the OBJ file
	LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath, nullptr);

	if (!warn.empty())
	{
		Logger::Log(spdlog::level::warn, warn.c_str());
	}

	if (!err.empty())
	{
		Logger::Log(spdlog::level::err, err.c_str());
		return false;
	}

	std::map<tinyobj::index_t, int, IndexComparator> uniqueVertices = {};

	for (size_t s = 0; s < shapes.size(); s++)
	{
		size_t indexOffset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			int fv = 3;

			for (size_t v = 0; v < fv; v++)
			{
				tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];

				// Check if vertex with these attributes was already processed
				if (!uniqueVertices.contains(idx))
				{
					// If not found
					// New unique Vertex
					Vertex newVertex;

					newVertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
					newVertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
					newVertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];

					newVertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
					newVertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
					newVertex.normal.z = attrib.normals[3 * idx.normal_index + 2];

					newVertex.color = glm::vec3(attrib.colors[3 * idx.vertex_index + 0],
					                             attrib.colors[3 * idx.vertex_index + 1],
					                             attrib.colors[3 * idx.vertex_index + 2]);

					newVertex.uv.x = attrib.texcoords[2 * idx.texcoord_index + 0];
					newVertex.uv.y = attrib.texcoords[2 * idx.texcoord_index + 1];

					// Save it to the vector and store the index in our map
					resultVertices.push_back(newVertex);
					uniqueVertices[idx] = static_cast<int>(resultVertices.size()) - 1;
				}

				// The vertex already exists in our vector, simply use the index
				resultIndices.push_back(uniqueVertices[idx]);
			}
			indexOffset += fv;
		}
	}

	return true;
}
