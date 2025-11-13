//
// Created by Remus on 9/01/2024.
//

#include "SkyboxRenderer.h"

#include "Base/Common/Data/PrimitiveMesh.h"
#include "Vulkan/Systems/GraphicsPipeline.h"

void SkyboxRenderer::Render(const VkCommandBuffer commandBuffer, const Scene& scene)
{
	mesh->Bind(commandBuffer);
	vkCmdPushConstants(commandBuffer, graphicsPipeline->pipelineConfig.pipelineLayout,
	                   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
	                   sizeof(PushConstants), &pushConstants);

	vkCmdDrawIndexed(commandBuffer, mesh->GetIndicesSize(), 1, 0, 0, 0);
	pushConstants.model = transform->GetWorldMatrix();
}

void SkyboxRenderer::BindRenderer(GraphicsPipeline& boundGraphicsPipeline)
{
	graphicsPipeline = &boundGraphicsPipeline;
	graphicsPipeline->AddRenderer(this);
}

void SkyboxRenderer::DestroyRenderer()
{
	delete mesh;
	Renderer::DestroyRenderer();
}

void SkyboxRenderer::LoadMesh(const char* path)
{
	mesh = new PrimitiveMesh();
	mesh->LoadFromObject(path);
}
