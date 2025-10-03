///////////////////////////////////////////////////////////////
// Created: 09.02.2025
// Author: NS_Deathman
// Renderer resetting realization
///////////////////////////////////////////////////////////////
#include "render_DX9.h"
#include "Log.h"
#include "filesystem.h"
#include "render_backend_DX9_deprecated.h"
#include "OptickAPI.h"
#include "helper_window.h"
#include "scene.h"
#include "user_interface.h"
#include "window_implementation.h"
#include "camera.h"
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
void CRenderDX9::Reset()
{
	Log("Resetting render...");

	OnResetBegin();

	ResetDirect3D();

	OnResetEnd();
}

void CRenderDX9::OnResetBegin()
{
	DummyWindow->SetResolution(g_ScreenWidth, g_ScreenHeight);
	DummyWindow->Reset();

	Camera->Reset();

	UserInterface->OnResetBegin();
}

void CRenderDX9::OnResetEnd()
{
	UserInterface->OnResetEnd();
}
///////////////////////////////////////////////////////////////
