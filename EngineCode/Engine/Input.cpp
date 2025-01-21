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
	m_KeyBoardStates = SDL_GetKeyboardState(NULL);

	// Initialize all keys as not pressed
	std::fill(std::begin(m_bKeyPressed), std::end(m_bKeyPressed), false);
}

void CInput::OnFrame()
{
	// Update the keyboard state
	m_KeyBoardStates = SDL_GetKeyboardState(NULL);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{ // Poll all events
		switch (event.type)
		{
		case SDL_KEYDOWN:
			// Set the corresponding key state to true
			m_bKeyPressed[event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			// Set the corresponding key state to false
			m_bKeyPressed[event.key.keysym.scancode] = false;
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
///////////////////////////////////////////////////////////////
