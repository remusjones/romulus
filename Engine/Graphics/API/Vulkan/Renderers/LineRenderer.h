//
// Created by Remus on 24/01/2024.
//

#pragma once
#include "Renderer.h"
#include "Base/Common/Data/Vertex.h"

class Color;
/*
 * Draws line(s) between "positions"
 */
class LineRenderer : public Renderer
{
public:
    enum LineRenderMode
    {
        // Draws a continuous connected line
        LINES_CONTINUOUS,
        // Draws a line between each position
        LINES_SEGMENTED,
    };

    void SetLinePositions(const std::vector<glm::vec3>& aPositions, LineRenderMode aMode = LINES_CONTINUOUS);

    void SetLinePositions(const std::vector<glm::vec3>& aPositions, const std::vector<Color>& aColors,
                          LineRenderMode aMode = LINES_CONTINUOUS);

    void DrawLine(glm::vec3 aStart, glm::vec3 aEnd, Color aColor);

    void DestroyRenderer() override;

    void BindRenderer(GraphicsPipeline& aBoundGraphicsPipeline) override;

    void Render(VkCommandBuffer aCommandBuffer, const Scene& aScene) override;

private:
    void SetLinePositions(const std::vector<Vertex>& aLines, LineRenderMode aMode = LINES_CONTINUOUS);

public:
    Transform* mTransform;

private:
    AllocatedBuffer* mAllocatedPositions;
    std::vector<Vertex> mLinePositions;

    uint32_t currentAllocationCount = 0;
    AllocatedBuffer* mTemporaryAllocatedPositions;
    std::vector<Vertex> mTemporaryLines;

    LineRenderMode mLineRenderMode{};
};
