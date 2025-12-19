///////////////////////////////////////////////////////////////
// Created: 19.01.2025
// Author: NS_Deathman
// Main window realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <windows.h>
#include <string>
#include "Core.h"
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
///////////////////////////////////////////////////////////////
namespace Core
{
	class CORE_API CWindow
	{
	  public:
		struct Config
		{
			uint32_t Width = 800;
			uint32_t Height = 600;
			std::string Name = "Window";
		};

		void Initialize(const Config& config);
		void Destroy();

		void CenterWindow();

		HWND GetWindow();
		SDL_Window* GetSDLWindow();

		SDL_DisplayMode GetSDLDisplayMode();

		void HideCursor();
		void ShowCursor();

		void Show();
		void Hide();

		void Reset();

		void SetResolution(uint32_t m_Width, uint32_t m_Height);
		void GetResolution(uint32_t* m_Width, uint32_t* m_Height)
		{
			m_Width = &this->m_Width;
			m_Height = &this->m_Height;
		}

		CWindow();
		~CWindow() = default;

	  private:
		SDL_Window* m_window;
		SDL_SysWMinfo m_WindowInfo;
		SDL_DisplayMode m_displayMode;
		SDL_Cursor* m_cursor;
		uint32_t m_Width;
		uint32_t m_Height;
		std::string m_WindowName;
		Config m_Config;

		void CreateSDLWindow();
		void DestroySDLWindow();

		void CreateCursor();
	};
} // namespace Core
///////////////////////////////////////////////////////////////
