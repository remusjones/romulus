//
// Created by Remus on 30/12/2023.
//

#pragma once
#include <SDL3/SDL_events.h>
#include "Camera.h"


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
	glm::vec3 MoveVector = {};
	bool Input[6]        = {};
	float_t Speed        = 10;
	bool RightMouseDown  = false;
};
