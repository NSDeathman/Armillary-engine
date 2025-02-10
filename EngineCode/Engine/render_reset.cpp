///////////////////////////////////////////////////////////////
// Created: 09.02.2025
// Author: NS_Deathman
// Renderer resetting realization
///////////////////////////////////////////////////////////////
#include "render.h"
#include "Log.h"
#include "filesystem.h"
#include "resource.h"
#include "render_backend.h"
#include "OptickAPI.h"
#include "helper_window.h"
#include "scene.h"
#include "user_interface.h"
#include "main_window.h"
#include "camera.h"
///////////////////////////////////////////////////////////////
extern UINT g_ScreenWidth;
extern UINT g_ScreenHeight;
///////////////////////////////////////////////////////////////
void CRender::Reset()
{
	Msg("Resetting render...");

	OnResetBegin();

	ResetDirect3D();

	OnResetEnd();
}

void CRender::OnResetBegin()
{
	SDL_SetWindowSize(MainWindow->GetSDLWindow(), g_ScreenWidth, g_ScreenHeight);
	MainWindow->CenterWindow();

	Camera->Reset();

	UserInterface->OnResetBegin();
}

void CRender::OnResetEnd()
{
	UserInterface->OnResetEnd();
}
///////////////////////////////////////////////////////////////
