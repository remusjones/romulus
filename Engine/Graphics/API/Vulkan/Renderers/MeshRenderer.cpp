//
// Created by Remus on 9/01/2024.
//

#include "MeshRenderer.h"

#include "Vulkan/Systems/GraphicsPipeline.h"

void MeshRenderer::LoadMesh(const char* aPath)
{
    // todo this should load from a global pool
    // eg. find or load mesh from path?
    mMesh = new Mesh();
    mMesh->LoadFromObject(aPath);
}

void MeshRenderer::DestroyRenderer()
{
    delete mMesh;
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
