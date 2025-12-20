///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: DeepSeek, ChatGPT, NS_Deathman
// Input realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Core.h"
#include "architect_patterns.h"
#include <SDL/SDL.h>
#include <array>
#include <algorithm>
#include "windows.h"
///////////////////////////////////////////////////////////////
#define DEFAULT_GAMEPAD_DEADZONE 0.2f
#define DEFAULT_GAMEPAD_SENSITIVITY 0.5f
#define DEFAULT_MOUSE_SENSITIVITY 1.0f
///////////////////////////////////////////////////////////////
namespace Core
{
	class CORE_API CInput : public Patterns::Singleton<CInput>
	{
		friend class Patterns::Singleton<CInput>;

		public:
		// Инициализация и завершение
		bool Initialize();
		void Destroy();
		bool IsInitialized() const
		{
			return m_Initialized;
		}

		// Основные методы
		void Update(); // Переименовано из OnFrame для ясности
		void HandleCursorWithGameController();

		// Клавиатура
		bool IsKeyPressed(int key);	 // Одно нажатие
		bool IsKeyHeld(int key);	 // Удержание
		bool IsKeyReleased(int key); // Отпускание

		// Геймпад
		bool IsGamepadConnected() const
		{
			return m_GameController != nullptr;
		}
		bool IsGamepadButtonPressed(int button);  // Одно нажатие
		bool IsGamepadButtonHeld(int button);	  // Удержание
		bool IsGamepadButtonReleased(int button); // Отпускание

		// Стики геймпада
		void GetLeftStick(float& x, float& y);
		void GetRightStick(float& x, float& y);
		float GetLeftTrigger();
		float GetRightTrigger();

		// Мышь
		void GetMouseDelta(int& deltaX, int& deltaY);
		void GetMousePosition(int& x, int& y);
		bool IsMouseButtonPressed(int button);
		bool IsMouseButtonHeld(int button);

		// Свойства
		float GetMouseSensitivity() const
		{
			return m_MouseSensitivity;
		}
		void SetMouseSensitivity(float sensitivity)
		{
			m_MouseSensitivity = std::max(0.1f, sensitivity);
		}

		float GetGamepadSensitivity() const
		{
			return m_GamepadSensitivity;
		}
		void SetGamepadSensitivity(float sensitivity)
		{
			m_GamepadSensitivity = std::max(0.1f, sensitivity);
		}

		float GetGamepadDeadzone() const
		{
			return m_GamepadDeadzone;
		}
		void SetGamepadDeadzone(float deadzone)
		{
			m_GamepadDeadzone = std::clamp(deadzone, 0.0f, 1.0f);
		}

		bool GetHandleCursorWithGameController() const
		{
			return m_HandleCursorWithGameController;
		}
		void SetHandleCursorWithGameController(bool enable)
		{
			m_HandleCursorWithGameController = enable;
		}

		// Виброотдача геймпада
		void VibrateGamepad(int durationMs, float strength);

	  private:
		CInput();
		~CInput();

		// Вспомогательные методы
		float ApplyDeadzone(float value, float deadzone);
		void UpdateKeyboard();
		void UpdateGamepad();
		void UpdateMouse();
		void ProcessSDLEvents();

		// Состояние ввода
		std::array<bool, SDL_NUM_SCANCODES> m_CurrentKeyState{};
		std::array<bool, SDL_NUM_SCANCODES> m_PreviousKeyState{};
		std::array<bool, SDL_CONTROLLER_BUTTON_MAX> m_CurrentGamepadButtonState{};
		std::array<bool, SDL_CONTROLLER_BUTTON_MAX> m_PreviousGamepadButtonState{};

		// Геймпад
		SDL_GameController* m_GameController = nullptr;
		SDL_JoystickID m_GameControllerInstanceID = -1;

		// Мышь
		int m_MouseX = 0;
		int m_MouseY = 0;
		int m_MouseDeltaX = 0;
		int m_MouseDeltaY = 0;
		Uint32 m_MouseButtonState = 0;
		Uint32 m_PreviousMouseButtonState = 0;

		// Настройки
		float m_GamepadDeadzone = DEFAULT_GAMEPAD_DEADZONE;
		float m_GamepadSensitivity = DEFAULT_GAMEPAD_SENSITIVITY;
		float m_MouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;

		// Флаги
		bool m_Initialized = false;
		bool m_HandleCursorWithGameController = false;

		// Для управления курсором через геймпад
		POINT m_LastCursorPosition{};
	};
} // namespace Core
///////////////////////////////////////////////////////////////
// Макросы для удобства
#define INPUT Core::CInput::GetInstance()

// Удобные макросы для ввода
#define KEY_PRESSED(key) Core::CInput::GetInstance().IsKeyPressed(key)
#define KEY_HELD(key) Core::CInput::GetInstance().IsKeyHeld(key)
#define KEY_RELEASED(key) Core::CInput::GetInstance().IsKeyReleased(key)

#define GAMEPAD_BUTTON_PRESSED(button) Core::CInput::GetInstance().IsGamepadButtonPressed(button)
#define GAMEPAD_BUTTON_HELD(button) Core::CInput::GetInstance().IsGamepadButtonHeld(button)
#define GAMEPAD_BUTTON_RELEASED(button) Core::CInput::GetInstance().IsGamepadButtonReleased(button)

#define MOUSE_BUTTON_PRESSED(button) Core::CInput::GetInstance().IsMouseButtonPressed(button)
#define MOUSE_BUTTON_HELD(button) Core::CInput::GetInstance().IsMouseButtonHeld(button)
///////////////////////////////////////////////////////////////
/*
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡶⠓⢷⡀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⠃⠀⠀⠘⣦⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢠⣴⣶⣶⣤⣠⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣾⣛⠇⠀⠀⠀⠀⣏⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢻⠀⠀⠈⠉⠻⣟⣾⣤⡀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣺⣟⡳⢿⠐⠠⡀⠀⠀⢸⡅⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢨⡟⠀⠀⠀⠀⠀⠈⠳⣭⢟⡷⣦⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣾⠷⣭⣛⢧⡈⠅⡒⠄⠀⢸⡁⠀⠀⠁⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⢸⣇⠀⠀⠀⠀⠀⠀⠀⠸⣿⣾⣿⣿⣦⡄⡀⠀⡀⣤⣴⣴⣿⠿⠷⠿⠿⠿⠿⠿⣶⣿⣿⣤⠸⣤⢻⡄⢸⡇⠀⠀⠀⠀⠀⠀
⠀⠀⡀⠀⠀⠀⠀⠈⣿⠀⠀⠀⠀⠀⠀⣀⠠⠘⣯⡟⣯⢟⣿⣧⡤⠟⠛⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠛⢧⣀⠏⡁⢘⡧⠀⠀⠀⠀⠀⠀
⠀⠀⠐⠃⠀⠀⠀⠐⢿⠀⠀⠀⠀⢀⠰⢀⠣⢁⡿⡽⣎⣿⡾⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⢿⣻⣧⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⣼⠀⠀⠀⠀⠆⢡⠊⢄⠃⣸⡗⣯⡛⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣴⣦⣝⢯⣄⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠹⡆⠀⠀⢈⠰⠁⡌⠢⢌⣷⢯⡓⠀⠀⣀⣤⣶⣶⣤⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣧⣽⣿⣿⡟⠀⣷⣤⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠰⢧⠀⠀⠠⡁⠎⡠⢑⡾⣝⣾⠀⠀⣾⣷⣤⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⠿⠿⠟⠋⠠⠄⠷⢿⣶⡀⠀⠀
⠀⠀⠀⠐⠀⠀⠀⠀⠀⠹⣇⠀⠀⠁⢂⣡⣾⢻⡜⣿⠀⠀⠙⠻⠿⢿⣿⣿⣿⣿⠇⠀⠀⠀⠀⡄⣠⢠⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⡷⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠩⣻⡗⣶⢫⡟⣮⢷⡹⣽⠲⠒⠀⠀⠀⠀⠢⡀⠀⠀⠀⠀⠀⠀⠀⠀⢣⠏⠀⠀⠀⠀⢀⣨⠰⠛⠁⠀⠀⠈⣛⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢹⣾⡝⣾⡱⣏⡷⣹⢇⠀⠀⠀⠀⠀⢔⡴⢀⢀⣀⣀⡀⣀⡤⠰⠁⠙⠡⠶⠜⠃⠁⠀⠀⠀⠀⠀⠂⠀⣯⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⢟⣾⡽⣎⢷⣭⣛⠷⣦⠀⠀⠀⠀⠀⠈⠉⠉⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠀⠀⠀⣸⠅⠀⠀
⠀⠀⠀⠀⠀⠐⠅⠀⠀⠀⠀⠀⠀⠀⢽⡿⣼⡹⣞⢶⣫⢟⡅⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠔⠁⠀⠀⣰⠏⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢘⣿⢳⢧⣻⢼⣫⢞⠝⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠔⠁⠀⠀⠀⡸⠇⠀⠀⠀⠀
⡤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⣯⣛⢮⡳⣏⡾⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⢶⣫⢏⡷⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡏⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣟⣮⠳⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⡄⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣼⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣂⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣨⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣯⡀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣧⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣘⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢷⠄⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣽⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⡗⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣴⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣇⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢘⡿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣏⡀
⠀⠀⠀⠀⠀⠀⠀⢀⣽⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⡁
⠀⠀⠀⠀⠀⠀⠀⠈⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣷
⠀⠀⠀⠀⠀⠀⠀⣾⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡧
⠀⠀⠀⠀⠀⠀⠀⣾⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣷
*/
///////////////////////////////////////////////////////////////
