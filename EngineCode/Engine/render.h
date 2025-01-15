///////////////////////////////////////////////////////////////
//Created: 15.01.2025
//Author: NS_Deathman
//Renderer realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////
class CRender
{
private:
	HWND m_hWindow;
	LPDIRECT3D9 m_pDirect3D;
	LPDIRECT3DDEVICE9 m_pDirect3dDevice;

	LPD3DXMESH m_pMesh;
	D3DMATERIAL9* m_pMeshMaterials;
	LPDIRECT3DTEXTURE9* m_pMeshTextures;
	DWORD m_dwNumMaterials;

public:
	void CreateMainWindow();
	void InitializeDirect3D();
	void CreateMatrices();
	void Initialize();
	void RenderFrame();

	void LoadGeometry();
	void LoadMaterials();
	void LoadTextures();
	void LoadScene();

	CRender();
	~CRender();
};
///////////////////////////////////////////////////////////////
extern CRender* Render;
///////////////////////////////////////////////////////////////


