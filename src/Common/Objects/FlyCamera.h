//
// Created by Remus on 30/12/2023.
//

#pragma once
#include <SDL3/SDL_events.h>
#include "Camera.h"
#include "EASTL/bitset.h"
#include "EASTL/bonus/flags.h"


struct KeyboardEvent;

class FlyCamera : public Camera
{
public:
	void Construct() override;
	void OnDebugGui() override;
	void Tick(float deltaTime) override;

	void Forward(const SDL_KeyboardEvent& keyboardEvent);
	void Backward(const SDL_KeyboardEvent& keyboardEvent);
	void Left(const SDL_KeyboardEvent& keyboardEvent);
	void Right(const SDL_KeyboardEvent& keyboardEvent);
	void Up(const SDL_KeyboardEvent& keyboardEvent);
	void Down(const SDL_KeyboardEvent& keyboardEvent);
	void MouseMovement(const SDL_MouseMotionEvent& mouseMotion);
	void MouseInput(const SDL_MouseButtonEvent& inMouseInput);

	bool IsCameraConsumingInput() const;

private:
	enum ECameraInput : uint8_t
	{
		FORWARD = 0,
		BACKWARD = 1,
		LEFT = 2,
		RIGHT = 3,
		UP = 4,
		DOWN = 5,
		RIGHT_MOUSE = 6
	};

	eastl::bitset<7> inputBitset;

	glm::vec3 moveVector		= {};

	float speed					= 10;
	float currentYaw			= 0.0f;
	float currentPitch			= 0.0f;
	float sensitivity			= 0.002f;
};
