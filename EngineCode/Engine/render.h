///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////
class CRender
{
  public:
	LPDIRECT3DDEVICE9 m_pDirect3dDevice;
	LPDIRECT3D9 m_pDirect3D;
	D3DPRESENT_PARAMETERS m_pDirect3DPresentParams;
	HWND m_hWindow;

	DWORD MaxSimultaneousTextures;
	
  private:
	LPD3DXMESH m_pMesh;
	std::vector<D3DMATERIAL9> m_pMeshMaterials;
	std::vector<LPDIRECT3DTEXTURE9> m_pMeshTextures;
	DWORD m_dwNumMaterials;

	BOOL m_bDeviceLost;

  public:
	void CreateMainWindow();
	void GetCapabilities();
	void InitializeDirect3D();
	void CreateMatrices();
	void Initialize();
	void HandleDeviceLost();

	void OnResetBegin();
	void Reset();
	void OnResetEnd();

	void Destroy();

	void OnFrameBegin();
	void RenderFrame();
	void OnFrameEnd();
	void OnFrame();

	void LoadGeometry();
	void LoadMaterials();
	void LoadTextures();
	void LoadScene();
	void RenderScene();

	CRender();
	~CRender() = default;
};
///////////////////////////////////////////////////////////////
extern CRender* Render;
///////////////////////////////////////////////////////////////
