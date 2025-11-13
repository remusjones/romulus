//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "Base/Common/Data/GPUCameraData.h"
#include "Entity.h"
#include "Math/Transform.h"

struct Ray;
class Camera : public Entity
{
public:
    // TODO: Only update vpm when m_transform gets marked dirty
    glm::mat4 GetViewProjectionMatrix() const;
    glm::mat4 GetPerspectiveMatrix() const;
    glm::mat4 GetViewMatrix() const;

    Ray GetRayTo(int x, int y);

    GPUCameraData GetCameraInformation() const;

    float_t fov = 70.0f;
    float_t zNear = 0.1f;
    float_t zFar = 200.0f;
};
