#include "InputSystem.h"
#include <imgui_impl_sdl3.h>

#include "tracy/Tracy.hpp"

InputSystem* gInputSystem = nullptr;

InputSystem::InputSystem()
{
	// todo: this should be using a singleton pattern instead
	assert(gInputSystem == nullptr);
	gInputSystem = this;
}

void InputSystem::ConsumeInput(const SDL_Event* inputEvent)
{
	ImGui_ImplSDL3_ProcessEvent(inputEvent);
	if (inputEvent->key.repeat == 0)
	{
		queuedEvents.push(*inputEvent);
	}
}

void InputSystem::ProcessInput(const SDL_Event* inputEvent)
{
	// todo: we are forwarding all input here, these callbacks aren't free
	if (inputEvent->key.repeat == 0)
	{
		switch (inputEvent->type)
		{
			case SDL_EVENT_KEY_DOWN:
				for (const auto& binding : keyboardBindings[inputEvent->key.key])
				{
					binding.callback(inputEvent->key);
				}
				break;
			case SDL_EVENT_KEY_UP:
				for (const auto& binding : keyboardBindings[inputEvent->key.key])
				{
					binding.callback(inputEvent->key);
				}
				break;
			case SDL_EVENT_MOUSE_MOTION:
				for (const auto& binding : mouseInputMotionBindings)
				{
					binding.callback(inputEvent->motion);
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				for (const auto& binding : mouseInputBindings)
				{
					binding.callback(inputEvent->button);
				}
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				for (const auto& binding : mouseInputBindings)
				{
					binding.callback(inputEvent->button);
				}
				break;
			default: break;
		}
	}
}

// todo: Filter out duplicate events, and queue only uniques so we can consume during Update
void InputSystem::Update()
{

	while (!queuedEvents.empty())
	{
		ProcessInput(&queuedEvents.front());
		queuedEvents.pop();
	}
}

void InputSystem::RegisterMouseInput(eastl::function<void(SDL_MouseMotionEvent)>&& callback,
                                     const char* bindingName)
{
	const MouseMotionBinding newBinding(bindingName, std::move(callback));
	mouseInputMotionBindings.push_back(newBinding);
}

void InputSystem::RegisterMouseInput(eastl::function<void(SDL_MouseButtonEvent)>&& callback, const char* bindingName)
{
	const MouseInputBinding newBinding(bindingName, std::move(callback));
	mouseInputBindings.push_back(newBinding);
}

void InputSystem::RegisterKeyCodeInput(const SDL_Keycode keycode,
                                       eastl::function<void(SDL_KeyboardEvent aKeyboardEvent)>&& callback,
                                       const char* bindingName)
{
	const KeyCodeInputBinding newBinding(bindingName, (std::move(callback)));
	keyboardBindings[keycode].push_back(newBinding);
}
