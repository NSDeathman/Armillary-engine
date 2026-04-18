#include "pch.h"
#include "Engine.h"
#include "Logger.h"

namespace Armillary
{

	Engine::Engine()
	{
		Logger::GetInstance().SetLogLevel(LogLevel::Debug);
		Logger::GetInstance().EnableConsoleOutput(true);
		Logger::GetInstance().EnableFileOutput("engine.log");
		LOG_INFO("CEngine constructor called");
	}

	Engine::~Engine()
	{
		LOG_INFO("CEngine destructor called");
	}

	bool Engine::Initialize()
	{
		LOG_INFO("Initializing CEngine...");
		LOG_INFO("CEngine initialized successfully");
		

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			LOG_ERROR("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
			return false;
		}

		// Создание окна
		SDL_Window* testWindow = SDL_CreateWindow(
			"Armillary Engine - SDL2 Test",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			800, 600,
			SDL_WINDOW_SHOWN
		);

		if (testWindow == nullptr)
		{
			LOG_ERROR("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
			return false;
		}

		LOG_INFO("SDL2 initialized and window created successfully!");

		// Задержка 3 секунды
		SDL_Delay(3000);

		// Уничтожение окна и выход
		SDL_DestroyWindow(testWindow);
		SDL_Quit();

		return true; // или false, если вы хотите сразу завершить программу после теста
	}

	void Engine::Run()
	{
		LOG_INFO("CEngine main loop started");
		std::cout << "Press Enter to exit..." << std::endl;
		m_bIsRunning = true;
		while (m_bIsRunning)
		{
			std::cin.get();
			m_bIsRunning = false;
		}
		LOG_INFO("CEngine main loop finished");
	}

	void Engine::Shutdown()
	{
		LOG_INFO("Shutting down CEngine...");
		LOG_INFO("CEngine shutdown complete");
	}

} // namespace Armillary