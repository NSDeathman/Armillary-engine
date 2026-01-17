///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Updated: [Текущая дата]
// Author: DeepSeek, ChatGPT, NS_Deathman
// Input realization - Enhanced for FPS games
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "AsyncExecutor.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <array>
#include "architect_patterns.h"
///////////////////////////////////////////////////////////////
#define DEFAULT_MOUSE_SENSITIVITY 1.0f
#define DEFAULT_GAMEPAD_SENSITIVITY 1.0f
#define DEFAULT_GAMEPAD_DEADZONE 0.4f
///////////////////////////////////////////////////////////////
CORE_BEGIN
class CInput
{
  public:
	CInput();
	~CInput();

	// Инициализация и завершение
	bool Initialize(int updateFrequency = 1000); // Частота обновления в герцах
	void Destroy();

	// Основные методы - такие же как у CInput для совместимости
	bool IsKeyPressed(int key);
	bool IsKeyHeld(int key);
	bool IsKeyReleased(int key);

	bool IsGamepadConnected() const;
	bool IsGamepadButtonPressed(int button);
	bool IsGamepadButtonHeld(int button);
	bool IsGamepadButtonReleased(int button);

	void GetLeftStick(float& x, float& y);
	void GetRightStick(float& x, float& y);
	float GetLeftTrigger();
	float GetRightTrigger();

	void GetMouseDelta(int& deltaX, int& deltaY);
	void GetMousePosition(int& x, int& y);
	bool IsMouseButtonPressed(int button);
	bool IsMouseButtonHeld(int button);

	// Настройки
	float GetMouseSensitivity() const;
	void SetMouseSensitivity(float sensitivity);

	float GetGamepadSensitivity() const;
	void SetGamepadSensitivity(float sensitivity);

	float GetGamepadDeadzone() const;
	void SetGamepadDeadzone(float deadzone);

	// Методы для обновления (вызываются из потока)
	void AsyncUpdate();

	// Синхронизация
	void BeginFrame();
	void EndFrame();

	// Статистика
	int GetUpdateFrequency() const
	{
		return m_UpdateFrequency;
	}
	double GetAverageUpdateTime() const
	{
		return m_AverageUpdateTime;
	}
	int GetDroppedFrames() const
	{
		return m_DroppedFrames;
	}

  private:
	// Структура для состояния ввода
	struct InputFrame
	{
		// Клавиатура
		std::array<bool, SDL_NUM_SCANCODES> KeyState{};
		std::array<bool, SDL_NUM_SCANCODES> PreviousKeyState{};

		// Геймпад
		bool GamepadConnected = false;
		std::array<bool, SDL_CONTROLLER_BUTTON_MAX> GamepadButtons{};
		std::array<bool, SDL_CONTROLLER_BUTTON_MAX> PreviousGamepadButtons{};

		float LeftStickX = 0.0f;
		float LeftStickY = 0.0f;
		float RightStickX = 0.0f;
		float RightStickY = 0.0f;
		float LeftTrigger = 0.0f;
		float RightTrigger = 0.0f;

		// Мышь
		int MouseX = 0;
		int MouseY = 0;
		int MouseDeltaX = 0;
		int MouseDeltaY = 0;
		Uint32 MouseButtons = 0;
		Uint32 PreviousMouseButtons = 0;

		// Метаданные
		uint64_t FrameNumber = 0;
		std::chrono::high_resolution_clock::time_point Timestamp;
	};

	// Потокобезопасное обновление состояний
	void UpdateKeyboardState(InputFrame& frame);
	void UpdateGamepadState(InputFrame& frame);
	void UpdateMouseState(InputFrame& frame);

	// Буферизация состояний
	static const int BUFFER_SIZE = 3;
	InputFrame m_FrameBuffer[BUFFER_SIZE];
	std::atomic<int> m_WriteIndex{0};
	std::atomic<int> m_ReadIndex{0};

	// Текущее состояние для чтения
	InputFrame m_CurrentFrame;

	// Потоковая обработка
	std::atomic<bool> m_Running{false};
	std::thread m_InputThread;
	std::mutex m_FrameMutex;
	std::condition_variable m_FrameCondition;

	// Настройки (потокобезопасные)
	std::atomic<float> m_MouseSensitivity{DEFAULT_MOUSE_SENSITIVITY};
	std::atomic<float> m_GamepadSensitivity{DEFAULT_GAMEPAD_SENSITIVITY};
	std::atomic<float> m_GamepadDeadzone{DEFAULT_GAMEPAD_DEADZONE};

	// Статистика
	std::atomic<int> m_UpdateFrequency{1000};
	std::atomic<double> m_AverageUpdateTime{0.0};
	std::atomic<int> m_DroppedFrames{0};
	std::atomic<uint64_t> m_FrameCounter{0};

	// Время
	std::chrono::high_resolution_clock::time_point m_LastUpdateTime;

	// Основной цикл потока
	void InputThreadFunc();
};
CORE_END
///////////////////////////////////////////////////////////////
