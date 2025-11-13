//
// Created by Remus on 23/12/2023.
//

#pragma once

#include <glm/glm.hpp>

struct GPUCameraData
{
	glm::mat4 mPerspectiveMatrix;
	glm::mat4 mViewMatrix;
	glm::mat4 mViewProjectionMatrix;
};
