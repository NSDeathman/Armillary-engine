///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: DeepSeek, ChatGPT, NS_Deathman
// Input realization
///////////////////////////////////////////////////////////////
#include "Input.h"
#include "log.h"
#include "window_implementation.h"
///////////////////////////////////////////////////////////////
namespace Core
{
	///////////////////////////////////////////////////////////////
	CInput::CInput()
	{
		// Конструктор вызывается только через GetInstance()
	}

	CInput::~CInput()
	{
		Destroy();
	}

	///////////////////////////////////////////////////////////////
	bool CInput::Initialize()
	{
		if (m_Initialized)
		{
			Log("Input system already initialized!");
			return true;
		}

		try
		{
			// Инициализация SDL подсистемы ввода
			if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK) < 0)
			{
				throw std::runtime_error(std::string("SDL input initialization failed: ") + SDL_GetError());
			}

			// Включение обработки событий геймпада
			SDL_GameControllerEventState(SDL_ENABLE);
			SDL_JoystickEventState(SDL_ENABLE);

			// Поиск подключенных геймпадов
			int numJoysticks = SDL_NumJoysticks();
			for (int i = 0; i < numJoysticks; ++i)
			{
				if (SDL_IsGameController(i))
				{
					m_GameController = SDL_GameControllerOpen(i);
					if (m_GameController)
					{
						m_GameControllerInstanceID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_GameController));
						Log("Game controller connected: %s", SDL_GameControllerName(m_GameController));
						break;
					}
				}
			}

			// Получение начальной позиции курсора
			GetCursorPos(&m_LastCursorPosition);

			m_Initialized = true;
			Log("Input system initialized successfully");
			Log("Gamepads detected: %d", numJoysticks);

			return true;
		}
		catch (const std::exception& e)
		{
			Log("Failed to initialize input system: %s", e.what());
			return false;
		}
	}

	///////////////////////////////////////////////////////////////
	void CInput::Destroy()
	{
		/*
		if (m_Initialized)
		{
			m_Initialized = false;

			Log("Destroying input system...");

			// Закрытие геймпада
			if (m_GameController)
			{
				SDL_GameControllerClose(m_GameController);

				// Выключение SDL подсистемы ввода
				SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);
			}
		}
		*/
	}

	///////////////////////////////////////////////////////////////
	void CInput::Update()
	{
		if (!m_Initialized)
			return;

		// Сохраняем предыдущее состояние
		m_PreviousKeyState = m_CurrentKeyState;
		m_PreviousGamepadButtonState = m_CurrentGamepadButtonState;
		m_PreviousMouseButtonState = m_MouseButtonState;

		// Обрабатываем события SDL
		ProcessSDLEvents();

		// Обновляем состояние устройств
		UpdateKeyboard();
		UpdateGamepad();
		UpdateMouse();

		// Обработка курсора через геймпад
		if (m_HandleCursorWithGameController)
		{
			HandleCursorWithGameController();
		}
	}

	///////////////////////////////////////////////////////////////
	void CInput::ProcessSDLEvents()
	{
		if (!m_Initialized)
			return;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_CONTROLLERDEVICEADDED:
				if (!m_GameController && SDL_IsGameController(event.cdevice.which))
				{
					m_GameController = SDL_GameControllerOpen(event.cdevice.which);
					if (m_GameController)
					{
						m_GameControllerInstanceID =
							SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(m_GameController));
						Log("Game controller connected: %s", SDL_GameControllerName(m_GameController));
					}
				}
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				if (m_GameController && event.cdevice.which == m_GameControllerInstanceID)
				{
					Log("Game controller disconnected: %s", SDL_GameControllerName(m_GameController));
					SDL_GameControllerClose(m_GameController);
					m_GameController = nullptr;
					m_GameControllerInstanceID = -1;
				}
				break;

			case SDL_MOUSEMOTION:
				m_MouseX = event.motion.x;
				m_MouseY = event.motion.y;
				m_MouseDeltaX = event.motion.xrel;
				m_MouseDeltaY = event.motion.yrel;
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				m_MouseButtonState = SDL_GetMouseState(nullptr, nullptr);
				break;
			}
		}
	}

	///////////////////////////////////////////////////////////////
	void CInput::UpdateKeyboard()
	{
		if (!m_Initialized)
			return;

		// Получаем текущее состояние клавиатуры
		int numKeys = 0;
		const Uint8* keyboardState = SDL_GetKeyboardState(&numKeys);

		for (int i = 0; i < numKeys && i < SDL_NUM_SCANCODES; ++i)
		{
			m_CurrentKeyState[i] = (keyboardState[i] != 0);
		}
	}

	///////////////////////////////////////////////////////////////
	void CInput::UpdateGamepad()
	{
		if (!m_Initialized)
			return;

		if (!m_GameController)
			return;

		// Обновляем состояние кнопок геймпада
		for (int button = 0; button < SDL_CONTROLLER_BUTTON_MAX; ++button)
		{
			m_CurrentGamepadButtonState[button] =
				(SDL_GameControllerGetButton(m_GameController, static_cast<SDL_GameControllerButton>(button)) ==
				 SDL_PRESSED);
		}
	}

	///////////////////////////////////////////////////////////////
	void CInput::UpdateMouse()
	{
		if (!m_Initialized)
			return;

		// Состояние мыши уже обновляется через события
		// Дополнительная логика может быть добавлена здесь
	}

	///////////////////////////////////////////////////////////////
	// Клавиатура
	bool CInput::IsKeyPressed(int key)
	{
		if (!m_Initialized)
			return false;

		if (key < 0 || key >= SDL_NUM_SCANCODES)
			return false;

		return m_CurrentKeyState[key] && !m_PreviousKeyState[key];
	}

	bool CInput::IsKeyHeld(int key)
	{
		if (!m_Initialized)
			return false;

		if (key < 0 || key >= SDL_NUM_SCANCODES)
			return false;

		return m_CurrentKeyState[key];
	}

	bool CInput::IsKeyReleased(int key)
	{
		if (!m_Initialized)
			return false;

		if (key < 0 || key >= SDL_NUM_SCANCODES)
			return false;

		return !m_CurrentKeyState[key] && m_PreviousKeyState[key];
	}

	///////////////////////////////////////////////////////////////
	// Геймпад
	bool CInput::IsGamepadButtonPressed(int button)
	{
		if (!m_Initialized)
			return false;

		if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX || !m_GameController)
			return false;

		return m_CurrentGamepadButtonState[button] && !m_PreviousGamepadButtonState[button];
	}

	bool CInput::IsGamepadButtonHeld(int button)
	{
		if (!m_Initialized)
			return false;

		if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX || !m_GameController)
			return false;

		return m_CurrentGamepadButtonState[button];
	}

	bool CInput::IsGamepadButtonReleased(int button)
	{
		if (!m_Initialized)
			return false;

		if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX || !m_GameController)
			return false;

		return !m_CurrentGamepadButtonState[button] && m_PreviousGamepadButtonState[button];
	}

	///////////////////////////////////////////////////////////////
	// Вспомогательные методы
	float CInput::ApplyDeadzone(float value, float deadzone)
	{
		if (!m_Initialized)
			return 0.0f;

		return (std::fabs(value) < deadzone) ? 0.0f : value;
	}

	///////////////////////////////////////////////////////////////
	// Стики геймпада
	void CInput::GetLeftStick(float& x, float& y)
	{
		x = y = 0.0f;

		if (!m_Initialized)
			return;

		if (!m_GameController)
			return;

		x = ApplyDeadzone(SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f,
						  m_GamepadDeadzone);
		y = ApplyDeadzone(SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f,
						  m_GamepadDeadzone);

		x *= m_GamepadSensitivity;
		y *= m_GamepadSensitivity;
	}

	void CInput::GetRightStick(float& x, float& y)
	{
		x = y = 0.0f;

		if (!m_Initialized)
			return;

		if (!m_GameController)
			return;

		x = ApplyDeadzone(SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f,
						  m_GamepadDeadzone);
		y = ApplyDeadzone(SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f,
						  m_GamepadDeadzone);

		x *= m_GamepadSensitivity;
		y *= m_GamepadSensitivity;
	}

	float CInput::GetLeftTrigger()
	{
		if (!m_Initialized)
			return 0.0f;

		if (!m_GameController)
			return 0.0f;

		return ApplyDeadzone(SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f,
							 m_GamepadDeadzone);
	}

	float CInput::GetRightTrigger()
	{
		if (!m_Initialized)
			return 0.0f;

		if (!m_GameController)
			return 0.0f;

		return ApplyDeadzone(SDL_GameControllerGetAxis(m_GameController, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f,
							 m_GamepadDeadzone);
	}

	///////////////////////////////////////////////////////////////
	// Мышь
	void CInput::GetMouseDelta(int& deltaX, int& deltaY)
	{
		if (!m_Initialized)
			return;

		deltaX = static_cast<int>(m_MouseDeltaX * m_MouseSensitivity);
		deltaY = static_cast<int>(m_MouseDeltaY * m_MouseSensitivity);
	}

	void CInput::GetMousePosition(int& x, int& y)
	{
		if (!m_Initialized)
			return;

		x = m_MouseX;
		y = m_MouseY;
	}

	bool CInput::IsMouseButtonPressed(int button)
	{
		if (!m_Initialized)
			return false;

		Uint32 mask = SDL_BUTTON(button);
		return (m_MouseButtonState & mask) && !(m_PreviousMouseButtonState & mask);
	}

	bool CInput::IsMouseButtonHeld(int button)
	{
		if (!m_Initialized)
			return 0.0f;

		return (m_MouseButtonState & SDL_BUTTON(button)) != 0;
	}

	///////////////////////////////////////////////////////////////
	// Управление курсором через геймпад
	void CInput::HandleCursorWithGameController()
	{
		if (!m_Initialized)
			return;

		if (!m_GameController)
			return;

		float leftStickX = 0.0f, leftStickY = 0.0f;
		GetLeftStick(leftStickX, leftStickY);

		GetCursorPos(&m_LastCursorPosition);

		// Корректируем позицию курсора на основе ввода со стика
		m_LastCursorPosition.x += static_cast<LONG>(leftStickX * 15.0f * m_GamepadSensitivity);
		m_LastCursorPosition.y += static_cast<LONG>(leftStickY * 15.0f * m_GamepadSensitivity);

		// Ограничиваем курсор в пределах экрана (опционально)
		SetCursorPos(m_LastCursorPosition.x, m_LastCursorPosition.y);

		// Эмуляция кликов мыши
		if (IsGamepadButtonHeld(SDL_CONTROLLER_BUTTON_A))
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, m_LastCursorPosition.x, m_LastCursorPosition.y, 0, 0);
		}

		if (IsGamepadButtonReleased(SDL_CONTROLLER_BUTTON_A))
		{
			mouse_event(MOUSEEVENTF_LEFTUP, m_LastCursorPosition.x, m_LastCursorPosition.y, 0, 0);
		}
	}

	///////////////////////////////////////////////////////////////
	// Виброотдача
	void CInput::VibrateGamepad(int durationMs, float strength)
	{
		if (!m_Initialized)
			return;

		if (!m_GameController)
			return;

		strength = std::clamp(strength, 0.0f, 1.0f);
		Uint16 rumbleStrength = static_cast<Uint16>(strength * 0xFFFF);

		SDL_GameControllerRumble(m_GameController, rumbleStrength, rumbleStrength, durationMs);
	}
	///////////////////////////////////////////////////////////////
} // namespace Core
///////////////////////////////////////////////////////////////