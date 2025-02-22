///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "render_DX9_stdafx.h"
///////////////////////////////////////////////////////////////
class CRenderDX9
{
  public:
	LPDIRECT3DDEVICE9 m_pDirect3dDevice;
	LPDIRECT3D9 m_pDirect3D;
	D3DPRESENT_PARAMETERS m_pDirect3DPresentParams;
	HWND m_hWindow;

	DWORD MaxSimultaneousTextures;
	D3DMULTISAMPLE_TYPE m_MaxMultiSamplingQuality;
	UINT m_Frame;

	IDirect3DVertexShader9* m_vertexShader;
	IDirect3DPixelShader9* m_pixelShader;
	ID3DXConstantTable* m_pConstantTable;
	
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

	CRenderDX9();
	~CRenderDX9() = default;
};
///////////////////////////////////////////////////////////////
#ifndef USE_DX11
///////////////////////////////////////////////////////////////
extern CRenderDX9* Render;
///////////////////////////////////////////////////////////////
#define Device ::Render->m_pDirect3dDevice
///////////////////////////////////////////////////////////////
#endif
///////////////////////////////////////////////////////////////
