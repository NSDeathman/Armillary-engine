///////////////////////////////////////////////////////////////
// Created: 23.09.2025
// Author: NS_Deathman
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Render.h"
///////////////////////////////////////////////////////////////
using namespace Core;
///////////////////////////////////////////////////////////////
namespace Render
{
	CRender::CRender() : m_CurrentAPI(GraphicsAPI::DirectX9)
	{
	}

	CRender::~CRender()
	{
		Shutdown();
	}

	bool CRender::Initialize(const Config& config)
	{
		m_Config = config;
		m_CurrentAPI = config.GraphicsAPI;

		CWindow::Config WindowConfig;

		WindowConfig.Width = config.Width;
		WindowConfig.Height = config.Height;
		WindowConfig.Name = "Armillary engine";

		m_Window.Initialize(WindowConfig);

		std::cout << "Initializing renderer with API: " << BackendFactory::APIToString(m_CurrentAPI) << std::endl;

		if (!CreateBackend())
		{
			std::cerr << "Failed to create graphics backend!" << std::endl;
			return false;
		}

		if (!m_Backend->Initialize(m_Window.GetWindow(), config.Width, config.Height))
		{
			std::cerr << "Failed to initialize graphics backend!" << std::endl;
			return false;
		}

		std::cout << "CRender initialized successfully!" << std::endl;
		return true;
	}

	bool CRender::CreateBackend()
	{
		m_Backend = BackendFactory::Create(m_CurrentAPI);
		return m_Backend != nullptr;
	}

	void CRender::Shutdown()
	{
		if (m_Backend)
		{
			m_Window.Hide();
			m_Backend->Shutdown();
			m_Backend.reset();
		}
	}

	void CRender::BeginFrame()
	{
		if (m_Backend)
		{
			m_Backend->BeginFrame();
			// Очистка экрана темно-синим цветом
			//m_Backend->Clear(0.0f, 0.1f, 0.3f, 1.0f);
		}
	}

	void CRender::EndFrame()
	{
		if (m_Backend)
		{
			m_Backend->EndFrame();
		}
	}

	void CRender::Present()
	{
		if (m_Backend)
		{
			m_Backend->Present();
		}
	}

	void CRender::Resize(uint32_t width, uint32_t height)
	{
		if (m_Backend)
		{
			m_Window.SetResolution(width, height);
			m_Backend->Resize(width, height);
		}
	}
} // namespace Render
///////////////////////////////////////////////////////////////
