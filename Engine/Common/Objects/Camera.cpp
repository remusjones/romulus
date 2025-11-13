//
// Created by Remus on 22/12/2023.
//

#include <Objects/Camera.h>
#include <VulkanGraphicsImpl.h>
#include <Physics/Ray.h>

glm::mat4 Camera::GetViewProjectionMatrix() const
{
	return GetPerspectiveMatrix() * GetViewMatrix();
}

glm::mat4 Camera::GetPerspectiveMatrix() const
{
	const glm::mat4 perspective =
		glm::perspective(glm::radians(fov),
		                 static_cast<float>(gGraphics->swapChain->
		                                               swapChainExtents.width) / static_cast<float>(gGraphics->swapChain
			                 ->
			                 swapChainExtents.height),
		                 zNear,
		                 zFar);

	return perspective;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::inverse(transform.GetTranslationMatrix() * transform.GetRotationMatrix());
}

Ray Camera::GetRayTo(const int x, const int y)
{
	const float width  = gGraphics->swapChain->swapChainExtents.width;
	const float height = gGraphics->swapChain->swapChainExtents.height;

	const float normalizedPointX = x / (width * 0.5f) - 1.0f;
	const float normalizedPointY = y / (height * 0.5f) - 1.0f;

	const glm::mat4 invVP     = glm::inverse(GetViewProjectionMatrix());
	const glm::vec4 screenPos = glm::vec4(normalizedPointX, -normalizedPointY, 1.0f, 1.0f);
	glm::vec4 worldPos        = invVP * screenPos;

	worldPos = worldPos / worldPos.w;

	Ray ray;
	ray.origin    = transform.GetWorldPosition();
	ray.direction = glm::normalize(glm::vec3(worldPos) - ray.origin);

	return ray;
}

GPUCameraData Camera::GetCameraInformation() const
{
	GPUCameraData information{};
	information.mViewProjectionMatrix = GetViewProjectionMatrix();
	information.mPerspectiveMatrix    = GetPerspectiveMatrix();
	information.mViewMatrix           = GetViewMatrix();
	return information;
}
