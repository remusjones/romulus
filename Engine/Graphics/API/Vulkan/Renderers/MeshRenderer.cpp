//
// Created by Remus on 9/01/2024.
//

#include "MeshRenderer.h"

#include "Base/Common/MeshAllocator.h"
#include "Vulkan/Systems/GraphicsPipeline.h"

void MeshRenderer::LoadMesh(MeshAllocator* inAllocator, const eastl::string_view& inPath)
{
    mMesh = inAllocator->LoadMesh(inPath);
}

void MeshRenderer::DestroyRenderer()
{
    Renderer::DestroyRenderer();
}

void MeshRenderer::BindRenderer(GraphicsPipeline& aBoundGraphicsPipeline)
{
    graphicsPipeline = &aBoundGraphicsPipeline;
    graphicsPipeline->AddRenderer(this);
}

void MeshRenderer::Render(VkCommandBuffer commandBuffer)
{
    mMesh->Bind(commandBuffer);
    pushConstants.model = mTransform->GetWorldMatrix();
    vkCmdPushConstants(commandBuffer, graphicsPipeline->pipelineConfig.pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(PushConstants), &pushConstants);

    vkCmdDrawIndexed(commandBuffer, mMesh->GetIndicesSize(), 1, 0, 0, 0);
    Renderer::Render(commandBuffer);
}
