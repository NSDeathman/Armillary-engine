///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Input realization
///////////////////////////////////////////////////////////////
#include "Input.h"
#include "log.h"
///////////////////////////////////////////////////////////////
CInput::CInput()
{
	Msg("Initializing input...");

	m_KeyBoardStates = SDL_GetKeyboardState(NULL);

	// Initialize all keys as not pressed
	std::fill(std::begin(m_bKeyPressed), std::end(m_bKeyPressed), false);
	std::fill(std::begin(m_bGamepadButtonPressed), std::end(m_bGamepadButtonPressed), false);

	// Open the first available game controller
	if (SDL_NumJoysticks() > 0)
	{
		//m_GameController = SDL_GameControllerOpen(0);
		if (m_GameController)
		{
			Msg("Already finded connected gamepad");
		}
	}
	else
	{
		m_GameController = nullptr; // No game controller available
	}

	m_bNeedUpdateInput = true;
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
		case SDL_CONTROLLERDEVICEADDED:
			Msg("Gamepad added");
			m_GameController = SDL_GameControllerOpen(events[i].cdevice.which);
			if (m_GameController)
				Msg("Gamepad controller opened successfuly");
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			Msg("Gamepad removed");
			if (m_GameController)
			{
				SDL_GameControllerClose(m_GameController);
				if (!m_GameController)
					Msg("Gamepad controller closed successfuly");
			}
			break;
		default:
			m_bNeedUpdateInput = false;
			break;
		}
	}
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

bool CInput::NeedUpdateInput()
{
	return true; // m_bNeedUpdateInput;
}
///////////////////////////////////////////////////////////////
