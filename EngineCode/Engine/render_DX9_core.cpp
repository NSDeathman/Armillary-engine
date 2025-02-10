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
extern UINT g_ScreenWidth;
extern UINT g_ScreenHeight;
///////////////////////////////////////////////////////////////
CRenderDX9::CRenderDX9()
{
	m_hWindow = nullptr;
	m_pDirect3D = nullptr;
	m_pDirect3dDevice = nullptr;
	ZeroMemory(&m_pDirect3DPresentParams, sizeof(m_pDirect3DPresentParams));

	m_bDeviceLost = false;
	m_bNeedReset = false;

	m_bWireframe = false;

	m_Frame = 0;
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

void CRenderDX9::CreateMatrices()
{
	D3DXMATRIX matView = Camera->GetViewMatrix();
	Device->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProjection = Camera->GetProjectionMatrix();
	Device->SetTransform(D3DTS_PROJECTION, &matProjection);

	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
	Device->SetTransform(D3DTS_WORLD, &matWorld);
}
///////////////////////////////////////////////////////////////
