///////////////////////////////////////////////////////////////
// Created: 10.02.2025
// Author: NS_Deathman
// Renderer frame pipeline realization
///////////////////////////////////////////////////////////////
#include "render_DX11.h"
#include "Log.h"
#include "filesystem.h"
#include "resource.h"
//#include "render_backend_DX11.h"
#include "OptickAPI.h"
#include "helper_window.h"
#include "scene.h"
#include "user_interface.h"
#include "main_window.h"
///////////////////////////////////////////////////////////////
extern UINT g_ScreenWidth;
extern UINT g_ScreenHeight;
///////////////////////////////////////////////////////////////
void CRenderDX11::OnFrame()
{
	OPTICK_EVENT("CRenderDX11::OnFrame")

	OnFrameBegin();
	RenderFrame();
	OnFrameEnd();
}

void CRenderDX11::OnFrameBegin()
{
	OPTICK_EVENT("CRenderDX11::OnFrameBegin")

		/*
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
		*/
}

void CRenderDX11::RenderFrame()
{
	OPTICK_EVENT("CRenderDX11::RenderFrame")

	UserInterface->Render();

	/*
	if (Scene->Ready())
		RenderScene();
		*/
}

void CRenderDX11::RenderScene()
{
	OPTICK_EVENT("CRenderDX11::RenderScene")

		/*
	// Turn on the zbuffer
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);

	RenderBackend->set_CullMode(CRenderBackendDX9::CULL_CCW);

	if (g_bWireframeMode)
		RenderBackend->set_FillMode(CRenderBackendDX9::FILL_WIREFRAME);

	Scene->DrawGeometry();

	if (g_bWireframeMode)
		RenderBackend->set_FillMode(CRenderBackendDX9::FILL_SOLID);
		*/
}

void CRenderDX11::OnFrameEnd()
{
	OPTICK_EVENT("CRenderDX11::OnFrameEnd")

	UserInterface->OnFrameEnd();

	/*
	// End the scene
	Device->EndScene();

	// Present the backbuffer contents to the display
	HRESULT present_result = Device->Present(NULL, NULL, NULL, NULL);

	if (present_result == D3DERR_DEVICELOST)
		m_bDeviceLost = true;
*/

	m_Frame++;
}
///////////////////////////////////////////////////////////////
