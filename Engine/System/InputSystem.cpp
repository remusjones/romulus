#include "InputSystem.h"
#include <imgui_impl_sdl3.h>

InputSystem* gInputSystem = nullptr;
InputSystem::InputSystem() {
    assert(gInputSystem == nullptr);
    gInputSystem = this;
}

void InputSystem::ConsumeInput(const SDL_Event *inputEvent)
{
    ImGui_ImplSDL3_ProcessEvent(inputEvent);
    if (inputEvent->key.repeat == 0)
    {
        queuedEvents.push(*inputEvent);
    }
}

void InputSystem::ProcessInput(const SDL_Event *inputEvent) {
    if (inputEvent->key.repeat == 0) {
        switch (inputEvent->type)
        {
            case SDL_EVENT_KEY_DOWN:
                for (const auto &binding: keyboardBindings[inputEvent->key.key]) {
                    binding.callback(inputEvent->key);
                }
            case SDL_EVENT_KEY_UP:
                for (const auto &binding: keyboardBindings[inputEvent->key.key]) {
                    binding.callback(inputEvent->key);
                }
            case SDL_EVENT_MOUSE_MOTION:
                for (const auto &binding: mouseInputMotionBindings) {
                    binding.callback(inputEvent->motion);
                }

            case SDL_EVENT_MOUSE_BUTTON_UP:
                for (const auto &binding: mouseInputBindings) {
                    binding.callback(inputEvent->button);
                }
            break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                for (const auto &binding: mouseInputBindings) {
                    binding.callback(inputEvent->button);
                }
            break;
            default: break;
        }
    }
}

// TODO: Propery filter out duplicate events, and queue only uniques so we can consume during Update
void InputSystem::Update() {
    while (!queuedEvents.empty()) {
        ProcessInput(&queuedEvents.front());
        queuedEvents.pop();
    }
}

void InputSystem::RegisterMouseInput(std::function<void(SDL_MouseMotionEvent)> &&callback,
                                      const char *bindingName) {
    const MouseMotionBinding newBinding(bindingName, std::move(callback));
    mouseInputMotionBindings.push_back(newBinding);
}

void InputSystem::RegisterMouseInput(std::function<void(SDL_MouseButtonEvent)> &&callback, const char *bindingName) {
    const MouseInputBinding newBinding(bindingName, std::move(callback));
    mouseInputBindings.push_back(newBinding);
}

void InputSystem::RegisterKeyCodeInput(const SDL_Keycode keycode,
                                        std::function<void(SDL_KeyboardEvent aKeyboardEvent)> &&callback,
                                        const char *bindingName) {
    const KeyCodeInputBinding newBinding(bindingName, (std::move(callback)));
    keyboardBindings[keycode].push_back(newBinding);
}
