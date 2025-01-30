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

void CRender::OnFrame()
{
	OnFrameBegin();
	RenderFrame();
	OnFrameEnd();
}

void CRender::RenderFrame()
{
	OPTICK_EVENT("CRender::RenderFrame")

	UserInterface->Render();

	if (Scene->Ready())
		RenderScene();
}

void CRender::OnFrameBegin()
{
	OPTICK_EVENT("CRender::OnFrameBegin")

	if (m_bDeviceLost)
		HandleDeviceLost();

	if (m_bNeedReset || g_bNeedRestart)
	{
		Reset();
		m_bNeedReset = false;
		g_bNeedRestart = false;
	}

	UserInterface->OnFrameBegin();

	// Handle window resize (we don't resize directly in the WM_SIZE handler)
	if (g_ScreenWidth != m_pDirect3DPresentParams.BackBufferWidth &&
		g_ScreenHeight != m_pDirect3DPresentParams.BackBufferHeight)
	{
		m_pDirect3DPresentParams.BackBufferWidth = g_ScreenWidth;
		m_pDirect3DPresentParams.BackBufferHeight = g_ScreenHeight;
		m_bNeedReset = true;
	}

	// Clear the backbuffer and the zbuffer
	D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)1.0f * 255, (int)1.0f * 255, (int)1.0f * 255, (int)1.0f * 255);

	Device->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

	// Setup the world, view, and projection matrices
	CreateMatrices();

	// Begin the scene
	HRESULT hresult = Device->BeginScene();

	if (FAILED(hresult))
		Msg("Failed to begin scene render");
}

void CRender::OnFrameEnd()
{
	UserInterface->OnFrameEnd();

	// End the scene
	Device->EndScene();

	// Present the backbuffer contents to the display
	HRESULT present_result = Device->Present(NULL, NULL, NULL, NULL);

	if (present_result == D3DERR_DEVICELOST)
		m_bDeviceLost = true;

	m_Frame++;
}

void CRender::RenderScene()
{
	// Turn on the zbuffer
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	RenderBackend->set_CullMode(CBackend::CULL_CCW);

	if (g_bWireframeMode)
		RenderBackend->set_FillMode(CBackend::FILL_WIREFRAME);

	Scene->DrawGeometry();

	if (g_bWireframeMode)
		RenderBackend->set_FillMode(CBackend::FILL_SOLID);
}
///////////////////////////////////////////////////////////////
