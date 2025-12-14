//
// Created by Remus on 9/01/2024.
//

#include "Renderer.h"

#include "VulkanGraphicsImpl.h"
#include "Base/Common/Material.h"

void Renderer::Render(VkCommandBuffer commandBuffer)
{
    if (material->propertiesBuffer.IsAllocated())
    {
        AllocatedBuffer::MapMemory(
            gGraphics->GetAllocator(), &material->materialProperties, material->propertiesBuffer.GetAllocation(),
            sizeof(MaterialProperties));
    }
}

void Renderer::BindRenderer(GraphicsPipeline& boundGraphicsPipeline)
{
}

void Renderer::DestroyRenderer()
{
}
