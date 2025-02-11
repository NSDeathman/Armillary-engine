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

	m_bNeedUpdateInput = false;
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
			//m_bNeedUpdateInput = true;
			break;
		case SDL_KEYUP:
			m_bKeyPressed[events[i].key.keysym.scancode] = false;
			//m_bNeedUpdateInput = false;
			break;
			// Handle other event types here if needed
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

bool CInput::NeedUpdateInput()
{
	return true; // m_bNeedUpdateInput;
}
///////////////////////////////////////////////////////////////
