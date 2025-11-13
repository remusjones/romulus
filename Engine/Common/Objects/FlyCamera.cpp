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
    Entity::Construct();
}

void FlyCamera::OnImGuiRender() {
    ImGui::Indent();
    Entity::OnImGuiRender();
    ImGui::SeparatorText("Settings");
    ImGui::DragFloat(GetUniqueLabel("FOV"), &fov, 0.1f);
    ImGui::DragFloat(GetUniqueLabel("Z Near"), &zNear, 0.01f);
    ImGui::DragFloat(GetUniqueLabel("Z Far"), &zFar, 0.1f);
    ImGui::SeparatorText("DEBUG");
    ImGui::DragFloat3(GetUniqueLabel("Forward"), &transform.Forward()[0], 0.1f);
    ImGui::DragFloat3(GetUniqueLabel("Right"), &transform.Right()[0], 0.1f);
    ImGui::DragFloat3(GetUniqueLabel("Up"), &transform.Up()[0], 0.1f);
    ImGui::Unindent();
}

void FlyCamera::Forward(const SDL_KeyboardEvent& keyboardEvent) {
    Input[0] = keyboardEvent.key == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Backward(const SDL_KeyboardEvent& keyboardEvent) {
    Input[1] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Left(const SDL_KeyboardEvent& keyboardEvent) {
    Input[2] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Right(const SDL_KeyboardEvent& keyboardEvent) {
    Input[3] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Up(const SDL_KeyboardEvent& keyboardEvent) {
    Input[4] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::Down(const SDL_KeyboardEvent& keyboardEvent) {
    Input[5] = keyboardEvent.type == SDL_EVENT_KEY_DOWN;
}

void FlyCamera::MouseMovement(const SDL_MouseMotionEvent &aMouseMotion) {
    if (!RightMouseDown)
        return;

    const float xDelta = aMouseMotion.xrel;
    const float yDelta = aMouseMotion.yrel;

    constexpr float sensitivity = 0.1f;
    const glm::vec2 delta = -glm::vec2(yDelta, xDelta) * sensitivity;
    transform.RotateAxisLocal(delta);
}

void FlyCamera::MouseInput(const SDL_MouseButtonEvent& InMouseInput)
{
    if (InMouseInput.button == SDL_BUTTON_RIGHT)
    {
        RightMouseDown = InMouseInput.type == SDL_EVENT_KEY_DOWN;
    }
}

bool FlyCamera::IsCameraConsumingInput() const
{
    return RightMouseDown;
}

glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
    return x * (1.f - t) + y * t;
}

void FlyCamera::Tick(float aDeltaTime) {
    // TODO: investigate global mouse state for off window input
    //float x, y;
    //LOG(INFO) << x << " " << y;
    if (IsCameraConsumingInput())
    {
        MoveVector = glm::vec3();
        MoveVector.z += Input[0] ? -Speed : 0;
        MoveVector.z += Input[1] ? Speed : 0;
        MoveVector.x += Input[2] ? -Speed : 0;
        MoveVector.x += Input[3] ? Speed : 0;
        MoveVector.y += Input[4] ? Speed : 0;
        MoveVector.y += Input[5] ? -Speed : 0;
        transform.TranslateLocal(MoveVector * aDeltaTime);
    } else
    {
        constexpr float dampeningSpeed = 25.f;
        MoveVector = lerp(MoveVector, glm::vec3(), aDeltaTime * dampeningSpeed);
        transform.TranslateLocal(MoveVector * aDeltaTime);
    }


    Entity::Tick(aDeltaTime);
}
