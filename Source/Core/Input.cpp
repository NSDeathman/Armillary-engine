///////////////////////////////////////////////////////////////
// Created: [Текущая дата]
// Author: NS_Deathman
// Async input system for FPS games
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Input.h"
#include "log.h"
#include <SDL/SDL.h>
#include <cmath>
///////////////////////////////////////////////////////////////
using namespace Core;
///////////////////////////////////////////////////////////////
CInput::CInput()
{
	m_LastUpdateTime = std::chrono::high_resolution_clock::now();
}

CInput::~CInput()
{
	Destroy();
}

///////////////////////////////////////////////////////////////
bool CInput::Initialize(int updateFrequency)
{
	if (m_Running)
	{
		Print("Async input system already initialized!");
		return true;
	}

	try
	{
		// Инициализация SDL подсистемы ввода
		if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) < 0)
		{
			throw std::runtime_error(std::string("SDL input initialization failed: ") + SDL_GetError());
		}

		// Включаем относительный режим мыши
		SDL_SetRelativeMouseMode(SDL_TRUE);

		// Настраиваем частоту обновления
		m_UpdateFrequency = std::clamp(updateFrequency, 60, 4000);

		// Инициализируем буферы
		for (int i = 0; i < BUFFER_SIZE; ++i)
		{
			m_FrameBuffer[i].FrameNumber = i;
			m_FrameBuffer[i].Timestamp = std::chrono::high_resolution_clock::now();
		}

		// Запускаем поток
		m_Running = true;
		m_InputThread = std::thread(&CInput::InputThreadFunc, this);

		// Устанавливаем приоритет потока
#ifdef _WIN32
		SetThreadPriority(m_InputThread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
#endif

		Print("Async input system initialized successfully (Update frequency: %d Hz)", (int)m_UpdateFrequency);
		return true;
	}
	catch (const std::exception& e)
	{
		Print("Failed to initialize async input system: %s", e.what());
		return false;
	}
}

///////////////////////////////////////////////////////////////
void CInput::Destroy()
{
	if (m_Running)
	{
		m_Running = false;
		m_FrameCondition.notify_all();

		if (m_InputThread.joinable())
		{
			m_InputThread.join();
		}

		// Отключаем относительный режим мыши
		SDL_SetRelativeMouseMode(SDL_FALSE);

		// Выключаем SDL подсистему ввода
		SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);

		Print("Async input system destroyed");
	}
}

///////////////////////////////////////////////////////////////
void CInput::InputThreadFunc()
{
	Print("Input thread started");

	const auto targetFrameTime = std::chrono::microseconds(1000000 / m_UpdateFrequency);

	while (m_Running)
	{
		auto frameStart = std::chrono::high_resolution_clock::now();

		// Обновляем состояние ввода
		AsyncUpdate();

		// Ожидаем следующего кадра
		auto frameEnd = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);

		if (elapsed < targetFrameTime)
		{
			std::this_thread::sleep_for(targetFrameTime - elapsed);
		}
		else
		{
			m_DroppedFrames++;
		}

		// Обновляем статистику
		auto updateTime = std::chrono::duration_cast<std::chrono::microseconds>(
							  std::chrono::high_resolution_clock::now() - frameStart)
							  .count() /
						  1000.0;

		// Экспоненциальное скользящее среднее
		m_AverageUpdateTime = 0.9 * m_AverageUpdateTime + 0.1 * updateTime;
	}

	Print("Input thread stopped");
}

///////////////////////////////////////////////////////////////
void CInput::AsyncUpdate()
{
	// Получаем индекс для записи
	int writeIndex = (m_WriteIndex.load() + 1) % BUFFER_SIZE;
	InputFrame& frame = m_FrameBuffer[writeIndex];

	// Сохраняем предыдущее состояние
	frame.PreviousKeyState = frame.KeyState;
	frame.PreviousGamepadButtons = frame.GamepadButtons;
	frame.PreviousMouseButtons = frame.MouseButtons;

	// Обновляем состояния
	UpdateKeyboardState(frame);
	UpdateGamepadState(frame);
	UpdateMouseState(frame);

	// Метаданные
	frame.FrameNumber = m_FrameCounter++;
	frame.Timestamp = std::chrono::high_resolution_clock::now();

	// Атомарно обновляем индекс записи
	m_WriteIndex.store(writeIndex);

	// Уведомляем об обновлении
	m_FrameCondition.notify_one();
}

///////////////////////////////////////////////////////////////
void CInput::UpdateKeyboardState(InputFrame& frame)
{
	// Получаем текущее состояние клавиатуры
	int numKeys = 0;
	const Uint8* keyboardState = SDL_GetKeyboardState(&numKeys);

	for (int i = 0; i < numKeys && i < SDL_NUM_SCANCODES; ++i)
	{
		frame.KeyState[i] = (keyboardState[i] != 0);
	}
}

///////////////////////////////////////////////////////////////
void CInput::UpdateGamepadState(InputFrame& frame)
{
	// Проверяем подключение геймпада
	int numJoysticks = SDL_NumJoysticks();
	frame.GamepadConnected = false;

	// Ищем первый подключенный геймпад
	for (int i = 0; i < numJoysticks; ++i)
	{
		if (SDL_IsGameController(i))
		{
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			if (controller)
			{
				frame.GamepadConnected = true;

				// Обновляем кнопки
				for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button)
				{
					frame.GamepadButtons[button] =
						(SDL_GameControllerGetButton(controller, static_cast<SDL_GameControllerButton>(button)) ==
						 SDL_PRESSED);
				}

				// Получаем значения осей
				float deadzone = m_GamepadDeadzone.load();

				// Левый стик
				float rawLX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
				float rawLY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;

				// Применяем deadzone
				auto applyDeadzone = [deadzone](float value) {
					if (fabsf(value) < deadzone)
						return 0.0f;
					float sign = (value > 0) ? 1.0f : -1.0f;
					return sign * ((fabsf(value) - deadzone) / (1.0f - deadzone));
				};

				frame.LeftStickX = applyDeadzone(rawLX);
				frame.LeftStickY = applyDeadzone(rawLY);

				// Правый стик
				float rawRX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
				float rawRY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f;

				frame.RightStickX = applyDeadzone(rawRX);
				frame.RightStickY = applyDeadzone(rawRY);

				// Триггеры
				frame.LeftTrigger =
					applyDeadzone(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f);
				frame.RightTrigger =
					applyDeadzone(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f);

				// Применяем чувствительность
				float sensitivity = m_GamepadSensitivity.load();
				frame.LeftStickX *= sensitivity;
				frame.LeftStickY *= sensitivity;
				frame.RightStickX *= sensitivity;
				frame.RightStickY *= sensitivity;

				SDL_GameControllerClose(controller);
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////
void CInput::UpdateMouseState(InputFrame& frame)
{
	// Получаем состояние мыши
	int x, y;
	Uint32 buttons = SDL_GetMouseState(&x, &y);

	frame.MouseX = x;
	frame.MouseY = y;
	frame.MouseButtons = buttons;

	// Для дельты мыши используем SDL_GetRelativeMouseState
	int deltaX, deltaY;
	SDL_GetRelativeMouseState(&deltaX, &deltaY);

	frame.MouseDeltaX = deltaX;
	frame.MouseDeltaY = deltaY;
}

///////////////////////////////////////////////////////////////
void CInput::BeginFrame()
{
	// Синхронизируем состояние с последним обновленным кадром
	std::unique_lock<std::mutex> lock(m_FrameMutex);

	// Ждем обновления если буфер пуст
	if (m_WriteIndex.load() == m_ReadIndex.load())
	{
		m_FrameCondition.wait_for(lock, std::chrono::milliseconds(1));
	}

	// Читаем последний доступный кадр
	int readIndex = m_WriteIndex.load();
	m_CurrentFrame = m_FrameBuffer[readIndex];
	m_ReadIndex.store(readIndex);
}

void CInput::EndFrame()
{
	// Сбрасываем дельту мыши для следующего кадра
	SDL_GetRelativeMouseState(nullptr, nullptr);
}

///////////////////////////////////////////////////////////////
// Методы чтения состояния
bool CInput::IsKeyPressed(int key)
{
	if (key < 0 || key >= SDL_NUM_SCANCODES)
		return false;

	return m_CurrentFrame.KeyState[key] && !m_CurrentFrame.PreviousKeyState[key];
}

bool CInput::IsKeyHeld(int key)
{
	if (key < 0 || key >= SDL_NUM_SCANCODES)
		return false;

	return m_CurrentFrame.KeyState[key];
}

bool CInput::IsKeyReleased(int key)
{
	if (key < 0 || key >= SDL_NUM_SCANCODES)
		return false;

	return !m_CurrentFrame.KeyState[key] && m_CurrentFrame.PreviousKeyState[key];
}

///////////////////////////////////////////////////////////////
bool CInput::IsGamepadConnected() const
{
	return m_CurrentFrame.GamepadConnected;
}

bool CInput::IsGamepadButtonPressed(int button)
{
	if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX)
		return false;

	return m_CurrentFrame.GamepadButtons[button] && !m_CurrentFrame.PreviousGamepadButtons[button];
}

bool CInput::IsGamepadButtonHeld(int button)
{
	if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX)
		return false;

	return m_CurrentFrame.GamepadButtons[button];
}

bool CInput::IsGamepadButtonReleased(int button)
{
	if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX)
		return false;

	return !m_CurrentFrame.GamepadButtons[button] && m_CurrentFrame.PreviousGamepadButtons[button];
}

///////////////////////////////////////////////////////////////
void CInput::GetLeftStick(float& x, float& y)
{
	x = m_CurrentFrame.LeftStickX;
	y = m_CurrentFrame.LeftStickY;
}

void CInput::GetRightStick(float& x, float& y)
{
	x = m_CurrentFrame.RightStickX;
	y = m_CurrentFrame.RightStickY;
}

float CInput::GetLeftTrigger()
{
	return m_CurrentFrame.LeftTrigger;
}

float CInput::GetRightTrigger()
{
	return m_CurrentFrame.RightTrigger;
}

///////////////////////////////////////////////////////////////
void CInput::GetMouseDelta(int& deltaX, int& deltaY)
{
	float sensitivity = m_MouseSensitivity.load();
	deltaX = static_cast<int>(m_CurrentFrame.MouseDeltaX * sensitivity);
	deltaY = static_cast<int>(m_CurrentFrame.MouseDeltaY * sensitivity);
}

void CInput::GetMousePosition(int& x, int& y)
{
	x = m_CurrentFrame.MouseX;
	y = m_CurrentFrame.MouseY;
}

bool CInput::IsMouseButtonPressed(int button)
{
	Uint32 mask = SDL_BUTTON(button);
	return (m_CurrentFrame.MouseButtons & mask) && !(m_CurrentFrame.PreviousMouseButtons & mask);
}

bool CInput::IsMouseButtonHeld(int button)
{
	Uint32 mask = SDL_BUTTON(button);
	return (m_CurrentFrame.MouseButtons & mask) != 0;
}

///////////////////////////////////////////////////////////////
float CInput::GetMouseSensitivity() const
{
	return m_MouseSensitivity.load();
}

void CInput::SetMouseSensitivity(float sensitivity)
{
	m_MouseSensitivity.store(std::max(0.01f, sensitivity));
}

float CInput::GetGamepadSensitivity() const
{
	return m_GamepadSensitivity.load();
}

void CInput::SetGamepadSensitivity(float sensitivity)
{
	m_GamepadSensitivity.store(std::max(0.1f, sensitivity));
}

float CInput::GetGamepadDeadzone() const
{
	return m_GamepadDeadzone.load();
}

void CInput::SetGamepadDeadzone(float deadzone)
{
	m_GamepadDeadzone.store(std::clamp(deadzone, 0.0f, 1.0f));
}
///////////////////////////////////////////////////////////////
