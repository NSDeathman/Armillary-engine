///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "render_DX9_stdafx.h"
#include "render_backend_DX9_deprecated.h"
///////////////////////////////////////////////////////////////
class CRenderDX9
{
  public:
	LPDIRECT3DDEVICE9 m_pDirect3dDevice;
	LPDIRECT3D9 m_pDirect3D;
	D3DPRESENT_PARAMETERS m_pDirect3DPresentParams;
	HWND m_hWindow;

	int MaxSimultaneousTextures;
	int MaxAnisotropy;
	int Anisotropy;
	D3DMULTISAMPLE_TYPE m_MaxMultiSamplingQuality;

	IDirect3DVertexShader9* m_vertexShader;
	IDirect3DPixelShader9* m_pixelShader;
	ID3DXConstantTable* m_pPixelShaderConstantTable;
	ID3DXConstantTable* m_pVertexShaderConstantTable;
	
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

	void SetAnisotropy(int value)
	{
		Anisotropy = value;
		RenderBackend->set_anisotropy_filtering(value);
	}

	CRenderDX9();
	~CRenderDX9() = default;
};
///////////////////////////////////////////////////////////////
extern CRenderDX9* RenderDeprecated;
///////////////////////////////////////////////////////////////
#define Device ::RenderDeprecated->m_pDirect3dDevice
///////////////////////////////////////////////////////////////
