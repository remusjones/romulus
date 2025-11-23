//
// Created by Remus on 30/12/2023.
//

#include "FlyCamera.h"

#include <glm/detail/type_quat.hpp>

#include "imgui.h"
#include "InputSystem.h"
#include "VulkanGraphicsImpl.h"

void FlyCamera::Construct()
{
	gInputSystem->RegisterKeyCodeInput(SDLK_W,
	                                   [this](const SDL_KeyboardEvent& kb) {
		                                   Forward(kb);
	                                   }, "Camera Forward");
	gInputSystem->RegisterKeyCodeInput(SDLK_S,
	                                   [&](const SDL_KeyboardEvent& kb) {
		                                   Backward(kb);
	                                   }, "Camera Backward");
	gInputSystem->RegisterKeyCodeInput(SDLK_D,
	                                   [&](const SDL_KeyboardEvent& kb) {
		                                   Right(kb);
	                                   }, "Camera Right");
	gInputSystem->RegisterKeyCodeInput(SDLK_A,
	                                   [&](const SDL_KeyboardEvent& kb) {
		                                   Left(kb);
	                                   }, "Camera Left");
	gInputSystem->RegisterKeyCodeInput(SDLK_SPACE,
	                                   [&](const SDL_KeyboardEvent& kb) {
		                                   Up(kb);
	                                   }, "Camera Up");
	gInputSystem->RegisterKeyCodeInput(SDLK_LCTRL,
	                                   [&](const SDL_KeyboardEvent& kb) {
		                                   Down(kb);
	                                   }, "Camera Down");

	gInputSystem->RegisterMouseInput([&](SDL_MouseMotionEvent motion) { MouseMovement(motion); }, "Camera Mouse");
	gInputSystem->RegisterMouseInput([&](SDL_MouseButtonEvent input) { MouseInput(input); }, "Camera Click");
	SceneObject::Construct();
}

void FlyCamera::OnDebugGui()
{
	ImGui::Indent();
	SceneObject::OnDebugGui();
	ImGui::SeparatorText("Settings");
	ImGui::DragFloat("FOV", &fov, 0.1f);
	ImGui::DragFloat("Z Near", &zNear, 0.01f);
	ImGui::DragFloat("Z Far", &zFar, 0.1f);
	ImGui::SeparatorText("DEBUG");
	ImGui::DragFloat3("Forward", &transform.Forward()[0], 0.1f);
	ImGui::DragFloat3("Right", &transform.Right()[0], 0.1f);
	ImGui::DragFloat3("Up", &transform.Up()[0], 0.1f);
	ImGui::Unindent();
}

void FlyCamera::Forward(const SDL_KeyboardEvent& keyboardEvent)
{
	inputBitset[ECameraInput::FORWARD] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Backward(const SDL_KeyboardEvent& keyboardEvent)
{
	inputBitset[ECameraInput::BACKWARD] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Left(const SDL_KeyboardEvent& keyboardEvent)
{
	inputBitset[ECameraInput::LEFT] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Right(const SDL_KeyboardEvent& keyboardEvent)
{
	inputBitset[ECameraInput::RIGHT] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Up(const SDL_KeyboardEvent& keyboardEvent)
{
	inputBitset[ECameraInput::UP] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Down(const SDL_KeyboardEvent& keyboardEvent)
{
	inputBitset[ECameraInput::DOWN] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::MouseMovement(const SDL_MouseMotionEvent& mouseMotion)
{
	if (!inputBitset[ECameraInput::RIGHT_MOUSE])
		return;

	const float deltaYaw   = -mouseMotion.xrel * sensitivity;
	const float deltaPitch = -mouseMotion.yrel * sensitivity;

	currentYaw   += deltaYaw;
	currentPitch += deltaPitch;

	constexpr float pitchLimit = glm::radians(89.0f);
	currentPitch = glm::clamp(currentPitch, -pitchLimit, pitchLimit);

	glm::quat qYaw   = glm::angleAxis(currentYaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qPitch = glm::angleAxis(currentPitch, glm::vec3(1.0f, 0.0f, 0.0f));

	transform.SetLocalRotation(qYaw * qPitch);
}

void FlyCamera::MouseInput(const SDL_MouseButtonEvent& inMouseInput)
{
	if (inMouseInput.button == SDL_BUTTON_RIGHT)
	{
		inputBitset[ECameraInput::RIGHT_MOUSE] = inMouseInput.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
	}
}

bool FlyCamera::IsCameraConsumingInput() const
{
	return inputBitset[ECameraInput::RIGHT_MOUSE];
}

glm::vec3 lerp(glm::vec3& x, glm::vec3& y, float t)
{
	return x * (1.f - t) + y * t;
}

void FlyCamera::Tick(float deltaTime)
{
	// TODO: investigate global mouse state for off window input
	//float x, y;
	//LOG(INFO) << x << " " << y;
	if (IsCameraConsumingInput())
	{
		moveVector = glm::vec3();
		moveVector.z += inputBitset[ECameraInput::FORWARD] ? -speed : 0;
		moveVector.z += inputBitset[ECameraInput::BACKWARD] ? speed : 0;
		moveVector.x += inputBitset[ECameraInput::LEFT] ? -speed : 0;
		moveVector.x += inputBitset[ECameraInput::RIGHT] ? speed : 0;
		moveVector.y += inputBitset[ECameraInput::UP] ? speed : 0;
		moveVector.y += inputBitset[ECameraInput::DOWN] ? -speed : 0;

		transform.TranslateLocal(moveVector * deltaTime);
	} else
	{
		constexpr float dampeningSpeed = 25.f;
		glm::vec3 zero = glm::zero<glm::vec3>();
		moveVector = lerp(moveVector, zero, deltaTime * dampeningSpeed);
		transform.TranslateLocal(moveVector * deltaTime);
	}


	SceneObject::Tick(deltaTime);
}
