///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Renderer realization
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
CRenderDX9::CRenderDX9()
{
	m_hWindow = nullptr;
	m_pDirect3D = nullptr;
	m_pDirect3dDevice = nullptr;
	m_pPixelShaderConstantTable = nullptr;
	m_pVertexShaderConstantTable = nullptr;
	ZeroMemory(&m_pDirect3DPresentParams, sizeof(m_pDirect3DPresentParams));

	m_bDeviceLost = false;
	m_bNeedReset = false;

	m_bWireframe = false;
	Anisotropy = 0;
}

void CRenderDX9::Initialize()
{
	Msg("Initializing render...");
	InitializeDirect3D();
}

void CRenderDX9::Destroy()
{
	Msg("Destroying render...");
	DestroyDirect3D();
}
///////////////////////////////////////////////////////////////
