///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Scene class
///////////////////////////////////////////////////////////////
#include "scene.h"
#include "render.h"
#include "log.h"
///////////////////////////////////////////////////////////////
CScene::CScene()
{
	m_pMesh = NULL;
	m_pMeshMaterials.resize(NULL);
	m_pMeshTextures.resize(NULL);
	m_dwNumMaterials = 0L;
	m_bSceneLoaded = false;
}

void CScene::Load()
{
	Log->Print("Loading scene...");

	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	HRESULT hresult = E_FAIL;

	hresult = D3DXLoadMeshFromX("..\\GameResources\\Tiger.x", D3DXMESH_SYSTEMMEM, Device, NULL,
								&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, &m_pMesh);

	// If model is not in current folder
	ASSERT(SUCCEEDED(hresult), "Could not find tiger.x")

	// We need to extract the material properties and texture names from the pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMeshMaterials.resize(m_dwNumMaterials);

	ASSERT(!m_pMeshMaterials.empty(), "Can`t load mesh materials")

	m_pMeshTextures.resize(m_dwNumMaterials);

	ASSERT(!m_pMeshTextures.empty(), "Can`t load mesh textures")

	concurrency::parallel_for(DWORD(0), m_dwNumMaterials, [&](u32 iterator) {
		// Copy the material
		m_pMeshMaterials[iterator] = d3dxMaterials[iterator].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		m_pMeshMaterials[iterator].Ambient = m_pMeshMaterials[iterator].Diffuse;

		m_pMeshTextures[iterator] = NULL;
		if (d3dxMaterials[iterator].pTextureFilename != NULL && d3dxMaterials[iterator].pTextureFilename[0] != '\0')
		{
			HRESULT hresult = E_FAIL;

			// Create texture path
			CHAR strTexture[MAX_PATH];
			strcpy_s(strTexture, MAX_PATH, GAME_RESOURCES);
			strcat_s(strTexture, MAX_PATH, d3dxMaterials[iterator].pTextureFilename);

			// Create the texture
			hresult = D3DXCreateTextureFromFileA(Device, strTexture, &m_pMeshTextures[iterator]);

			ASSERT(SUCCEEDED(hresult), "Could not find texture map")
		}
	});

	// Done with the material buffer
	pD3DXMtrlBuffer->Release();

	Log->Print("Scene loaded successfully");

	SetSceneLoaded(true);
}

void CScene::DrawGeometry()
{
	// Meshes are divided into subsets, one for each material. Render them in a loop
	concurrency::parallel_for(DWORD(0), m_dwNumMaterials, [this](u32 iterator) {
		// Set the material and texture for this subset
		Device->SetMaterial(&m_pMeshMaterials[iterator]);
		Device->SetTexture(0, m_pMeshTextures[iterator]);

		// Draw the mesh subset
		m_pMesh->DrawSubset(iterator);
	});
}

void CScene::Destroy()
{
	Log->Print("Destroying scene...");

	if (!m_pMeshTextures.empty())
	{
		concurrency::parallel_for(DWORD(0), m_dwNumMaterials, [this](u32 iterator) {
			if (m_pMeshTextures[iterator])
				m_pMeshTextures[iterator]->Release();
		});

		m_pMeshTextures.clear();
	}

	if (!m_pMeshMaterials.empty())
		m_pMeshMaterials.clear();

	SetSceneLoaded(false);
}
///////////////////////////////////////////////////////////////
