///////////////////////////////////////////////////////////////
// Created: 09.02.2025
// Author: NS_Deathman
// Renderer frame pipeline realization
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
void CRenderDX9::OnFrame()
{
	OPTICK_EVENT("CRenderDX9::OnFrame")

	OnFrameBegin();
	RenderFrame();
	OnFrameEnd();
}

void CRenderDX9::OnFrameBegin()
{
	OPTICK_EVENT("CRenderDX9::OnFrameBegin")

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
		g_ScreenHeight != m_pDirect3DPresentParams.BackBufferHeight) [[unlikely]]
	{
		m_pDirect3DPresentParams.BackBufferWidth = g_ScreenWidth;
		m_pDirect3DPresentParams.BackBufferHeight = g_ScreenHeight;
		m_bNeedReset = true;
	}

	m_pDirect3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	// Begin the scene
	HRESULT hresult = m_pDirect3dDevice->BeginScene();

	// Setup the world, view, and projection matrices
	CreateMatrices();

	if (FAILED(hresult))
		Msg("Failed to begin scene render");
}

void CRenderDX9::CreateMatrices()
{
	D3DXMATRIX matView = Camera->GetViewMatrix();
	m_pVertexShaderConstantTable->SetMatrix(Device, "matView", &matView);

	D3DXMATRIX matProjection = Camera->GetProjectionMatrix();
	m_pVertexShaderConstantTable->SetMatrix(Device, "matProjection", &matProjection);

	D3DXMATRIX matWorld;
	//D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
	D3DXMatrixTranslation(&matWorld, 0, 0, 0);
	m_pVertexShaderConstantTable->SetMatrix(Device, "matWorld", &matWorld);

	D3DXMATRIX matWorldViewProjection = matWorld * matView * matProjection;
	m_pVertexShaderConstantTable->SetMatrix(Device, "matWorldViewProjection", &matWorldViewProjection);

	D3DXMATRIX matViewProjection = matView * matProjection;
	D3DXMATRIX matViewProjectionTransposed;
	D3DXMatrixTranspose(&matViewProjectionTransposed, &matWorldViewProjection);
	m_pVertexShaderConstantTable->SetMatrix(Device, "matViewProjection", &matViewProjection);

	D3DXMATRIX matWorldView;
	D3DXMATRIX matWorldViewTransposed;
	matWorldView = matWorld * matView;
	D3DXMatrixTranspose(&matWorldViewTransposed, &matWorldView);
	m_pVertexShaderConstantTable->SetMatrix(Device, "matWorldView", &matWorldView);
}

void CRenderDX9::RenderFrame()
{
	OPTICK_EVENT("CRenderDX9::RenderFrame")

	UserInterface->Render();

	if (Scene->Ready())
		RenderScene();
}

void CRenderDX9::RenderScene()
{
	OPTICK_EVENT("CRenderDX9::RenderScene")

	m_pDirect3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	RenderBackend->set_CullMode(CRenderBackendDX9::CULL_FRONT);

	RenderBackend->enable_anisotropy_filtering();

	if (g_bWireframeMode)
		RenderBackend->set_FillMode(CRenderBackendDX9::FILL_WIREFRAME);

	Scene->DrawGeometry();

	m_pDirect3dDevice->SetVertexShader(m_vertexShader);
	m_pDirect3dDevice->SetPixelShader(m_pixelShader);

	if (g_bWireframeMode)
		RenderBackend->set_FillMode(CRenderBackendDX9::FILL_SOLID);

	RenderBackend->disable_anisotropy_filtering();
}

void CRenderDX9::OnFrameEnd()
{
	OPTICK_EVENT("CRenderDX9::OnFrameEnd")

	UserInterface->OnFrameEnd();

	// End the scene
	m_pDirect3dDevice->EndScene();

	// Present the backbuffer contents to the display
	HRESULT present_result = m_pDirect3dDevice->Present(NULL, NULL, NULL, NULL);

	if (present_result == D3DERR_DEVICELOST)
		m_bDeviceLost = true;
}
///////////////////////////////////////////////////////////////
