///////////////////////////////////////////////////////////////
// Created: 19.01.2025
// Author: NS_Deathman
// Main window realization
///////////////////////////////////////////////////////////////
#include "window_implementation.h"
#include "log.h"
///////////////////////////////////////////////////////////////
namespace Core
{
	SDL_WindowFlags g_WindowFlags = (SDL_WindowFlags) ( SDL_WINDOW_SHOWN | 
														SDL_WINDOW_INPUT_FOCUS | 
														SDL_WINDOW_BORDERLESS |
														SDL_WINDOW_MAXIMIZED | 
														SDL_WINDOW_ALLOW_HIGHDPI );
	
	CWindow::CWindow()
	{
		this->m_Width = 800;
		this->m_Height = 600;
	}

	void CWindow::Initialize(const Config& config)
	{
		Log("Creating window...");

		this->m_Width = config.Width;
		this->m_Height = config.Height;
		this->m_WindowName = config.Name;

		this->CreateSDLWindow();

		this->CreateCursor();
	}

	void CWindow::Destroy()
	{
		Log("Destroying window...");

		this->DestroySDLWindow();
	}

	void CWindow::CreateSDLWindow()
	{
		m_window = SDL_CreateWindow(m_WindowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, g_WindowFlags);

		SDL_VERSION(&m_WindowInfo.version);

		HRESULT result = SDL_GetWindowWMInfo(m_window, &m_WindowInfo);

		if (result == E_FAIL)
		{
			Log("! Failed to retrieve SDL window handle: %s", SDL_GetError());
			return;
		}

		// Get the current display mode
		result = SDL_GetCurrentDisplayMode(0, &m_displayMode);

		if (result == E_FAIL)
		{
			Log("! Failed to get SDL display mode: %s", SDL_GetError());
			return;
		}
	}

	void CWindow::CenterWindow()
	{
		if (!m_window)
		{
			Log("! Cannot center window: window is not created.");
			return;
		}

		// Calculate center position
		int centerX = (m_displayMode.w - m_Width) / 2;
		int centerY = (m_displayMode.h - m_Height) / 2;

		SDL_SetWindowPosition(m_window, centerX, centerY);

		DbgLog("Window centered at %d, %d", centerX, centerY);
	}

	void CWindow::DestroySDLWindow()
	{
		if (!m_window)
			return;

		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	HWND CWindow::GetWindow()
	{
		return m_WindowInfo.info.win.window;
	}

	SDL_Window* CWindow::GetSDLWindow()
	{
		return m_window;
	}

	SDL_DisplayMode CWindow::GetSDLDisplayMode()
	{
		return m_displayMode;
	}

	void CWindow::Reset()
	{
		this->CenterWindow();
	}

	void CWindow::SetResolution(uint32_t m_Width, uint32_t m_Height)
	{
		this->m_Width = m_Width;
		this->m_Height = m_Height;

		SDL_SetWindowSize(GetSDLWindow(), this->m_Width, this->m_Height);

		this->CenterWindow();
	}

	void CWindow::CreateCursor()
	{
		Uint8 l_data[1];
		Uint8 l_mask[1];

		l_data[0] = 0;
		l_mask[0] = 0;

		m_cursor = SDL_CreateCursor(l_data, l_mask, 1, 1, 0, 0);
		SDL_SetCursor(m_cursor);
		SDL_ShowCursor(true);
	}

	void CWindow::HideCursor()
	{
		SDL_ShowCursor(false);
	}

	void CWindow::ShowCursor()
	{
		SDL_ShowCursor(true);
	}

	void CWindow::Show()
	{
		SDL_ShowWindow(m_window);
	}

	void CWindow::Hide()
	{
		SDL_HideWindow(m_window);
	}
} // namespace Core
///////////////////////////////////////////////////////////////
