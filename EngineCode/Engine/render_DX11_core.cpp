///////////////////////////////////////////////////////////////
// Created: 10.02.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#include "render_DX11.h"
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
CRenderDX11::CRenderDX11()
{
	m_hWindow = nullptr;
	m_pDirect3dDevice = nullptr;
	m_pDirect3dDeviceContext = nullptr;
	m_pDirect3dSwapChain = nullptr;
	ZeroMemory(&m_pDirect3dSwapChainDescription, sizeof(m_pDirect3dSwapChainDescription));

	m_bDeviceLost = false;
	m_bNeedReset = false;

	m_bWireframe = false;

	m_Frame = 0;
}

void CRenderDX11::Initialize()
{
	Msg("Initializing render...");
	InitializeDirect3D();
}

void CRenderDX11::Destroy()
{
	Msg("Destroying render...");
	DestroyDirect3D();
}

void CRenderDX11::CreateMatrices()
{
	//D3DXMATRIX matView = Camera->GetViewMatrix();
	//Device->SetTransform(D3DTS_VIEW, &matView);

	//D3DXMATRIX matProjection = Camera->GetProjectionMatrix();
	//Device->SetTransform(D3DTS_PROJECTION, &matProjection);

	//D3DXMATRIXA16 matWorld;
	//D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
	//Device->SetTransform(D3DTS_WORLD, &matWorld);
}
///////////////////////////////////////////////////////////////
