///////////////////////////////////////////////////////////////
// Created: 24.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#include "Core.h"
#include "render_backend_interface.h"
#include "render_backend_DX9.h"
#include <memory>
#include <string>
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
namespace Render
{
	// Перечисление поддерживаемых графических API
	enum class CORE_API GraphicsAPI
	{
		DirectX9,
		DirectX11,
		OpenGL,
		Vulkan,
		AutoDetect // Автоматическое определение лучшего API
	};

	class CORE_API BackendFactory
	{
	  public:
		// Основной метод создания бэкенда
		static std::unique_ptr<IRenderBackend> Create(GraphicsAPI api);

		// Вспомогательные методы
		static GraphicsAPI DetectBestAPI();
		static std::string APIToString(GraphicsAPI api);
		static bool IsAPISupported(GraphicsAPI api);

	  private:
		// Приватные методы создания конкретных бэкендов
		static std::unique_ptr<IRenderBackend> CreateDX9();
		static std::unique_ptr<IRenderBackend> CreateDX11();
		static std::unique_ptr<IRenderBackend> CreateOpenGL();
		static std::unique_ptr<IRenderBackend> CreateVulkan();
	};
} // namespace Render
///////////////////////////////////////////////////////////////
