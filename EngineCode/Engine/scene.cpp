///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Scene class
///////////////////////////////////////////////////////////////
#include "scene.h"
#include "log.h"
#include "threading.h"

#ifdef USE_DX11
#include "render_DX11.h"
#else
#include "render_DX9.h"
#endif
///////////////////////////////////////////////////////////////
CScene::CScene()
{
	m_bSceneLoadingInProcess = false;
}

void LoadMesh()
{
	Scene->SetSceneLoadingState(true);

	Scene->m_MeshLoader.Create(Device, MESHES, "maxwell.obj");

	Scene->SetSceneLoadingState(false);
	Scene->SetSceneLoaded(true);

	Msg("Scene loaded successfully");
}

void DestroyMesh()
{
	Scene->m_MeshLoader.Destroy();

	Scene->SetSceneLoaded(false);

	Msg("Scene destroyed successfully");
}

void CScene::Load()
{
	Msg("Loading scene...");

	Scheduler->Add(LoadMesh);
}

void CScene::DrawGeometry()
{
	// Meshes are divided into subsets, one for each material. Render them in a loop
	for (UINT iSubset = 0; iSubset < m_MeshLoader.GetNumMaterials(); iSubset++) 
	{
		ID3DXMesh* pMesh = m_MeshLoader.GetMesh();
		if (pMesh)
		{
			Material* pMaterial = m_MeshLoader.GetMaterial(iSubset);
			Device->SetTexture(0, pMaterial->pTexture);
			pMesh->DrawSubset(iSubset);
		}
	}
}

void CScene::Destroy()
{
	Msg("Destroying scene...");

	DestroyMesh();
}
///////////////////////////////////////////////////////////////
