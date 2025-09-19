///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Input realization
///////////////////////////////////////////////////////////////
#include "Input.h"
#include "log.h"
#include "main_window.h"
///////////////////////////////////////////////////////////////
CInput::CInput()
{
	Msg("Initializing input...");

	m_KeyBoardStates = SDL_GetKeyboardState(NULL);

	std::fill(std::begin(m_bKeyPressed), std::end(m_bKeyPressed), false);
	std::fill(std::begin(m_bGamepadButtonPressed), std::end(m_bGamepadButtonPressed), false);

	m_GameController = nullptr;

	m_bNeedUpdateInput = true;
	m_bNeedHandleCursorWithGameController = false;

	m_gamepad_deadzone = DEFAULT_GAMEPAD_DEADZONE;
	m_gamepad_sensivity = DEFAULT_GAMEPAD_SENSIVITY;
	m_mouse_sensivity = DEFAULT_MOUSE_SENSIVITY;

	GetCursorPos(&m_ptLastCursorPosition);
}

void CInput::HandleCursorWithGameController()
{
	if (m_GameController && m_bNeedHandleCursorWithGameController)
	{
		float LeftStickX = 0.0f;
		float LeftStickY = 0.0f;

		GetLeftStick(LeftStickX, LeftStickY);

		GetCursorPos(&m_ptLastCursorPosition);

		// Adjust the cursor position based on the right stick input
		m_ptLastCursorPosition.x += static_cast<LONG>(LeftStickX * 10.0f); // Adjust scaling factor as needed
		m_ptLastCursorPosition.y += static_cast<LONG>(LeftStickY * 10.0f); // Subtract y-axis because screen coordinates typically have 0 at the top

		SetCursorPos(m_ptLastCursorPosition.x, m_ptLastCursorPosition.y);

		if (GamepadButtonHolded(SDL_CONTROLLER_BUTTON_A))
			mouse_event(MOUSEEVENTF_LEFTDOWN, m_ptLastCursorPosition.x, m_ptLastCursorPosition.y, 0, 0);

		if (GamepadButtonReleased(SDL_CONTROLLER_BUTTON_A))
			mouse_event(MOUSEEVENTF_LEFTUP, m_ptLastCursorPosition.x, m_ptLastCursorPosition.y, 0, 0);
	}
}

void CInput::OnFrame()
{
	// Update the keyboard state
	m_KeyBoardStates = SDL_GetKeyboardState(NULL);

	// Peep events from the queue
	SDL_Event events[10]; // Array to store events
	int numEvents = SDL_PeepEvents(events, 10, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);

	for (int i = 0; i < numEvents; ++i)
	{
		switch (events[i].type)
		{
		case SDL_KEYDOWN:
			m_bKeyPressed[events[i].key.keysym.scancode] = true;
			m_bNeedUpdateInput = true;
			break;
		case SDL_KEYUP:
			m_bKeyPressed[events[i].key.keysym.scancode] = false;
			m_bNeedUpdateInput = false;
			break;
		default:
			m_bNeedUpdateInput = false;
			break;
		}

		if (SDL_NumJoysticks() && !m_GameController)
		{
			m_GameController = SDL_GameControllerOpen(events[i].cdevice.which);
			if (m_GameController)
				Msg("New game controller device opened successfuly");
		}
		else if (SDL_NumJoysticks() == 0 && m_GameController)
		{
			SDL_GameControllerClose(m_GameController);
			m_GameController = nullptr; // Set to null after closing
			Msg("Game controller device closed successfuly");
		}
	}

	HandleCursorWithGameController();
}

bool CInput::KeyPressed(int key)
{
	// Check if the key is currently pressed
	if (m_KeyBoardStates[key])
	{
		// If it wasn't already marked as pressed, it means this is the first press
		if (!m_bKeyPressed[key])
		{
			// Update the pressed state to true
			m_bKeyPressed[key] = true;
			// Return true indicating the key was just pressed
			return true;
		}
	}
	else
	{
		// If the key is not pressed, reset its state for next frame
		m_bKeyPressed[key] = false;
	}

	// Return false indicating the key was not just pressed
	return false;
}

bool CInput::KeyHolded(int key)
{
	// Return true if the key is currently being held down
	return m_KeyBoardStates[key];
}

bool CInput::GamepadButtonPressed(int button)
{
	// Check if the gamepad button is pressed
	if (m_GameController)
	{
		if (SDL_GameControllerGetButton(m_GameController, static_cast<SDL_GameControllerButton>(button)) == SDL_PRESSED)
		{
			// If it wasn't already marked as pressed, it means this is the first press
			if (!m_bGamepadButtonPressed[button])
			{
				// Update the pressed state to true
				m_bGamepadButtonPressed[button] = true;
				// Return true indicating the key was just pressed
				return true;
			}
		}
		else
		{
			// If the key is not pressed, reset its state for next frame
			m_bGamepadButtonPressed[button] = false;
		}
	}
	return false;
}

bool CInput::GamepadButtonHolded(int button)
{
	// Return true if the gamepad button is being held down
	return m_GameController && SDL_GameControllerGetButton(m_GameController, static_cast<SDL_GameControllerButton>(button)) == SDL_PRESSED;
}

bool CInput::GamepadButtonReleased(int button)
{
	// Return true if the gamepad button is being held down
	return m_GameController && SDL_GameControllerGetButton(m_GameController, static_cast<SDL_GameControllerButton>(button)) == SDL_RELEASED;
}

float Length(float x, float y)
{
	return std::sqrt(x * x + y * y);
}

// New method to get left stick position
void CInput::GetLeftStick(float& x, float& y)
{
	if (m_GameController)
	{
		x = SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f; // Normalize to [-1, 1]
		y = SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f; // Normalize to [-1, 1]

		if (Length(x, y) < m_gamepad_deadzone)
		{
			x = y = 0.0f;
		}

		x *= m_gamepad_sensivity;
		y *= m_gamepad_sensivity;
	}
	else
	{
		x = 0.0f;
		y = 0.0f;
	}
}

// New method to get right stick position
void CInput::GetRightStick(float& x, float& y)
{
	if (m_GameController)
	{
		x = SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f; // Normalize to [-1, 1]
		y = SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f; // Normalize to [-1, 1]

		if (Length(x, y) < m_gamepad_deadzone)
		{
			x = y = 0.0f;
		}

		x *= m_gamepad_sensivity;
		y *= m_gamepad_sensivity;
	}
	else
	{
		x = 0.0f;
		y = 0.0f;
	}
}

bool CInput::NeedUpdateInput()
{
	return true; // m_bNeedUpdateInput;
}
///////////////////////////////////////////////////////////////
CInput* Input = nullptr;
///////////////////////////////////////////////////////////////
