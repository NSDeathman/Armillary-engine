///////////////////////////////////////////////////////////////
// Created: 24.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#include "render_backend_factory.h"
///////////////////////////////////////////////////////////////
namespace Render
{
	std::unique_ptr<IRenderBackend> BackendFactory::Create(GraphicsAPI api)
	{
		// Обработка авто-определения
		if (api == GraphicsAPI::AutoDetect)
		{
			api = DetectBestAPI();
		}

		switch (api)
		{
		case GraphicsAPI::DirectX9:
			return CreateDX9();

		case GraphicsAPI::DirectX11:
			// return CreateDX11(); // Пока заглушка
			break;

		case GraphicsAPI::OpenGL:
			// return CreateOpenGL(); // Пока заглушка
			break;

		case GraphicsAPI::Vulkan:
			// return CreateVulkan(); // Пока заглушка
			break;
		}

		// Fallback на DX9 если запрошенный API не поддерживается
		return CreateDX9();
	}

	std::unique_ptr<IRenderBackend> BackendFactory::CreateDX9()
	{
		return std::make_unique<RenderBackendDX9>();
	}

	GraphicsAPI BackendFactory::DetectBestAPI()
	{
		// Простая логика авто-определения:
		// 1. Проверить Windows версию
		// 2. Проверить наличие DLL для разных API
		// 3. Вернуть самый современный поддерживаемый API

		// Пока всегда возвращаем DX9 для простоты
		return GraphicsAPI::DirectX9;
	}

	string BackendFactory::APIToString(GraphicsAPI api)
	{
		switch (api)
		{
		case GraphicsAPI::DirectX9:
			return "DirectX 9";
		case GraphicsAPI::DirectX11:
			return "DirectX 11";
		case GraphicsAPI::OpenGL:
			return "OpenGL";
		case GraphicsAPI::Vulkan:
			return "Vulkan";
		default:
			return "Unknown";
		}
	}

	bool BackendFactory::IsAPISupported(GraphicsAPI api)
	{
		// Упрощенная проверка - пока поддерживаем только DX9
		return api == GraphicsAPI::DirectX9;
	}
} // namespace Render
///////////////////////////////////////////////////////////////
