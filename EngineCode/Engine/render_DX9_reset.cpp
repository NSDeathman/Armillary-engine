///////////////////////////////////////////////////////////////
// Created: 09.02.2025
// Author: NS_Deathman
// Renderer resetting realization
///////////////////////////////////////////////////////////////
#include "render_DX9.h"
#include "Log.h"
#include "filesystem.h"
#include "resource.h"
#include "render_backend_DX9.h"
#include "OptickAPI.h"
#include "helper_window.h"
#include "scene.h"
#include "user_interface.h"
#include "main_window.h"
#include "camera.h"
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
void CRenderDX9::Reset()
{
	Msg("Resetting render...");

	OnResetBegin();

	ResetDirect3D();

	OnResetEnd();
}

void CRenderDX9::OnResetBegin()
{
	SDL_SetWindowSize(MainWindow->GetSDLWindow(), g_ScreenWidth, g_ScreenHeight);

	MainWindow->CenterWindow();

	Camera->Reset();

	UserInterface->OnResetBegin();
}

void CRenderDX9::OnResetEnd()
{
	UserInterface->OnResetEnd();
}
///////////////////////////////////////////////////////////////
