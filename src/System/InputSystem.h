#pragma once
#include <functional>
#include <queue>
#include <unordered_map>
#include <SDL3/SDL_events.h>

#include "EASTL/hash_map.h"
#include "EASTL/queue.h"
#include "EASTL/vector.h"
#include "EASTL/internal/function.h"

struct KeyCodeInputBinding
{
    KeyCodeInputBinding(const char* inBindingName,
                        std::function<void(SDL_KeyboardEvent keyboardEvent)>&& inCallback) : bindingName(inBindingName),
        callback(inCallback) {
    };

    const char* bindingName;
    eastl::function<void(SDL_KeyboardEvent keyboardEvent)> callback;
};

struct MouseMotionBinding
{
    MouseMotionBinding(const char* inBindingName,
                       eastl::function<void(SDL_MouseMotionEvent aMouseMotionEvent)>&& inCallback) : bindingName(
            inBindingName),
        callback(inCallback) {
    };

    const char* bindingName;
    eastl::function<void(SDL_MouseMotionEvent)> callback;
};

struct MouseInputBinding
{
    MouseInputBinding(const char* bindingName,
                       eastl::function<void(SDL_MouseButtonEvent mouseButtonEvent)>&& inCallback) : bindingName(
            bindingName),
        callback(inCallback) {
    };

    const char* bindingName;
    eastl::function<void(SDL_MouseButtonEvent)> callback;
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
        eastl::function<void(SDL_KeyboardEvent aKeyboardEvent)>&& callback,
        const char* bindingName = "Default"
    );

    void RegisterMouseInput(
        eastl::function<void(SDL_MouseMotionEvent)>&& callback,
        const char* bindingName = "Default");

    void RegisterMouseInput(
        eastl::function<void(SDL_MouseButtonEvent)>&& callback,
        const char* bindingName = "Default");

    // TODO: make descriptor map
    eastl::hash_map<uint32_t, std::vector<KeyCodeInputBinding>> keyboardBindings;

    // TODO: make map
    eastl::vector<MouseMotionBinding> mouseInputMotionBindings;
    eastl::vector<MouseInputBinding> mouseInputBindings;

    eastl::queue<SDL_Event> queuedEvents;
};

extern InputSystem *gInputSystem;