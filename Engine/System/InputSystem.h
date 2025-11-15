#pragma once
#include <functional>
#include <queue>
#include <SDL3/SDL_events.h>
#include <unordered_map>

#include "Base/Common/Buffers/Texture.h"

struct KeyCodeInputBinding
{
    KeyCodeInputBinding(const char* inBindingName,
                        std::function<void(SDL_KeyboardEvent keyboardEvent)>&& inCallback) : bindingName(inBindingName),
        callback(inCallback) {
    };

    const char* bindingName;
    std::function<void(SDL_KeyboardEvent keyboardEvent)> callback;
};

struct MouseMotionBinding
{
    MouseMotionBinding(const char* inBindingName,
                       std::function<void(SDL_MouseMotionEvent aMouseMotionEvent)>&& inCallback) : bindingName(
            inBindingName),
        callback(inCallback) {
    };

    const char* bindingName;
    std::function<void(SDL_MouseMotionEvent)> callback;
};

struct MouseInputBinding
{
    MouseInputBinding(const char* bindingName,
                       std::function<void(SDL_MouseButtonEvent mouseButtonEvent)>&& inCallback) : bindingName(
            bindingName),
        callback(inCallback) {
    };

    const char* bindingName;
    std::function<void(SDL_MouseButtonEvent)> callback;
};

// TODO: Add Input event map bindings
class InputSystem
{
public:
    InputSystem();

    void ConsumeInput(const SDL_Event* inputEvent);
    void ProcessInput(const SDL_Event* inputEvent);
    void Update();

    void RegisterKeyCodeInput(
        SDL_Keycode keycode,
        std::function<void(SDL_KeyboardEvent aKeyboardEvent)>&& callback,
        const char* bindingName = "Default"
    );

    void RegisterMouseInput(
        std::function<void(SDL_MouseMotionEvent)>&& callback,
        const char* bindingName = "Default");

    void RegisterMouseInput(
        std::function<void(SDL_MouseButtonEvent)>&& callback,
        const char* bindingName = "Default");

    // TODO: make descriptor map
    std::unordered_map<uint32_t, std::vector<KeyCodeInputBinding>> keyboardBindings;

    // TODO: make map
    std::vector<MouseMotionBinding> mouseInputMotionBindings;
    std::vector<MouseInputBinding> mouseInputBindings;

    std::queue<SDL_Event> queuedEvents;
};

extern InputSystem *gInputSystem;