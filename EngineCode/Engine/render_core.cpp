///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Renderer realization
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
CRender::CRender()
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

void CRender::Initialize()
{
	Msg("Initializing render...");
	InitializeDirect3D();
}

void CRender::Destroy()
{
	Msg("Destroying render...");
	DestroyDirect3D();
}

void CRender::CreateMatrices()
{
	// Get view and projection matrices
	D3DXMATRIX view = Camera->GetViewMatrix();
	Device->SetTransform(D3DTS_VIEW, &view);

	D3DXMATRIX projection = Camera->GetProjectionMatrix();
	Device->SetTransform(D3DTS_PROJECTION, &projection);

	// Set up world matrix
	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
	Device->SetTransform(D3DTS_WORLD, &matWorld);
}
///////////////////////////////////////////////////////////////
