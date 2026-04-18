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
		return true;
	}

	void Engine::Run()
	{
		LOG_INFO("CEngine main loop started");
		std::cout << "Press Enter to exit..." << std::endl;
		std::cin.get();
		LOG_INFO("CEngine main loop finished");
	}

	void Engine::Shutdown()
	{
		LOG_INFO("Shutting down CEngine...");
		LOG_INFO("CEngine shutdown complete");
	}

} // namespace Armillary