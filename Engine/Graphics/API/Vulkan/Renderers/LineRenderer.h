//
// Created by Remus on 24/01/2024.
//

#pragma once
#include "Renderer.h"
#include "Base/Common/Buffers/AllocatedBuffer.h"
#include "EASTL/vector.h"

class Transform;
class Color;
/*
 * Draws line(s) between "positions"
 */
class LineRenderer final : public Renderer
{
public:
    enum LineRenderMode
    {
        // Draws a continuous connected line
        LINES_CONTINUOUS,
        // Draws a line between each position
        LINES_SEGMENTED,
    };

    void SetLinePositions(const eastl::vector<glm::vec3>& aPositions, LineRenderMode aMode = LINES_CONTINUOUS);

    void SetLinePositions(const eastl::vector<glm::vec3>& aPositions, const eastl::vector<Color>& aColors,
                          LineRenderMode aMode = LINES_CONTINUOUS);

    void DrawLine(const glm::vec3& aStart, const glm::vec3& aEnd, const Color& aColor);
    void DestroyRenderer() override;
    void BindRenderer(GraphicsPipeline& aBoundGraphicsPipeline) override;
    void Render(VkCommandBuffer aCommandBuffer) override;

public:
    Transform* mTransform;

private:
    AllocatedBuffer* mAllocatedPositions;
    AllocatedBuffer* mTemporaryAllocatedPositions;

    eastl::vector<Vertex> mLinePositions;
    eastl::vector<Vertex> mTemporaryLines;

    uint32_t currentAllocationCount = 0;
    LineRenderMode mLineRenderMode{};
};
