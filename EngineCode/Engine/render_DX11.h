///////////////////////////////////////////////////////////////
// Created: 10.02.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "render_DX11_stdafx.h"
///////////////////////////////////////////////////////////////
class CRenderDX11
{
  public:
	ID3D11Device* m_pDirect3dDevice;
	ID3D11DeviceContext* m_pDirect3dDeviceContext;
	IDXGISwapChain* m_pDirect3dSwapChain;
	ID3D11RenderTargetView* m_pDirect3dRenderTargetView;
	DXGI_SWAP_CHAIN_DESC m_pDirect3dSwapChainDescription;
	HWND m_hWindow;

	//DWORD MaxSimultaneousTextures;
	//D3DMULTISAMPLE_TYPE m_MaxMultiSamplingQuality;
	UINT m_Frame;

  private:
	BOOL m_bDeviceLost;
	BOOL m_bNeedReset;

	BOOL m_bWireframe;

	void GetCapabilities();
	void InitializeDirect3D();
	void DestroyDirect3D();
	void ResetDirect3D();

	void CreateMatrices();
	void HandleDeviceLost();

	void OnResetBegin();
	void OnResetEnd();

	void OnFrameBegin();
	void RenderFrame();
	void OnFrameEnd();
	void RenderScene();

  public:
	void Initialize();
	void Reset();
	void Destroy();
	void OnFrame();
	void SetNeedReset()
	{
		m_bNeedReset = true;
	}

	CRenderDX11();
	~CRenderDX11() = default;
};
///////////////////////////////////////////////////////////////
#ifdef USE_DX11
///////////////////////////////////////////////////////////////
extern CRenderDX11* Render;
///////////////////////////////////////////////////////////////
#define Device ::Render->m_pDirect3dDevice
#define DeviceContext ::Render->m_pDirect3dDeviceContext
///////////////////////////////////////////////////////////////
#endif
///////////////////////////////////////////////////////////////
