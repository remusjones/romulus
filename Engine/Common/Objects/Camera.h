//
// Created by Remus on 22/12/2023.
//

#pragma once
#include "Base/Common/Data/GPUCameraData.h"
#include "SceneObject.h"
#include "Math/Transform.h"

struct Ray;

class Camera : public SceneObject
{
public:
	// TODO: Only update vpm when m_transform gets marked dirty
	glm::mat4 GetViewProjectionMatrix();
	glm::mat4 GetPerspectiveMatrix();
	glm::mat4 GetViewMatrix();

	Ray GetRayTo(int x, int y);

	GPUCameraData GetCameraInformation();

	float_t fov   = 70.0f;
	float_t zNear = 0.1f;
	float_t zFar  = 200.0f;
};
