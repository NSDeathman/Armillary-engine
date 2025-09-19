///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: ChatGPT, NS_Deathman
// Input realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#define DEFAULT_GAMEPAD_DEADZONE 0.2f
#define DEFAULT_GAMEPAD_SENSIVITY 0.5f
#define DEFAULT_MOUSE_SENSIVITY 1.0f
///////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////
class CInput
{
  private:
	const Uint8* m_KeyBoardStates;

	// Array to track key pressed states
	bool m_bKeyPressed[SDL_NUM_SCANCODES];
	bool m_bGamepadButtonPressed[SDL_CONTROLLER_BUTTON_MAX];

	SDL_GameController* m_GameController; // Pointer to the game controller

	bool m_bNeedUpdateInput;
	bool m_bNeedHandleCursorWithGameController;

	POINT m_ptLastCursorPosition;
	
	float m_mouse_sensivity;
	
	float m_gamepad_deadzone;
	float m_gamepad_sensivity;

  public:
	CInput();
	~CInput() = default;

	void HandleCursorWithGameController();

	void OnFrame();

	bool KeyPressed(int key);
	bool KeyHolded(int key);

	bool GamepadButtonPressed(int button);
	bool GamepadButtonHolded(int button);
	bool GamepadButtonReleased(int button);

	void GetLeftStick(float& x, float& y);
	void GetRightStick(float& x, float& y);

	bool NeedUpdateInput();
	void SetNeedUpdateCursorWithGameController(bool flag)
	{
		m_bNeedHandleCursorWithGameController = flag;
	}

	float GetMouseSensivity()
	{
		return m_mouse_sensivity;
	}

	void SetMouseSensivity(float mouse_sensivity)
	{
		this->m_mouse_sensivity = mouse_sensivity;
	}

	float GetGamepadSensivity()
	{
		return m_gamepad_sensivity;
	}

	void SetGamepadSensivity(float gamepad_sensivity)
	{
		this->m_gamepad_sensivity = gamepad_sensivity;
	}

	float GetGamepadDeadzone()
	{
		return m_gamepad_deadzone;
	}

	void SetGamepadDeadzone(float gamepad_deadzone)
	{
		this->m_gamepad_deadzone = gamepad_deadzone;
	}
};
///////////////////////////////////////////////////////////////
extern CInput* Input;
///////////////////////////////////////////////////////////////
