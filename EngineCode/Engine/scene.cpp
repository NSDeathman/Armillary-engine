///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Scene class
///////////////////////////////////////////////////////////////
#include "scene.h"
#include "render.h"
#include "log.h"
#include "threading.h"
///////////////////////////////////////////////////////////////
CScene::CScene()
{
	m_bSceneLoadingInProcess = false;
}

void LoadMesh()
{
	Scene->SetSceneLoadingState(true);

	Scene->m_MeshLoader.Create(Device, "Earth.obj");
	Msg("Scene loaded successfully");

	Scene->SetSceneLoadingState(false);
	Scene->SetSceneLoaded(true);
}

void CScene::Load()
{
	Msg("Loading scene...");

	Scheduler->Add(LoadMesh);
}

void CScene::DrawGeometry()
{
	// Meshes are divided into subsets, one for each material. Render them in a loop
	concurrency::parallel_for(UINT(0), m_MeshLoader.GetNumMaterials(), [this](UINT iSubset) 
	{
		ID3DXMesh* pMesh = m_MeshLoader.GetMesh();
		if (pMesh)
		{
			Material* pMaterial = m_MeshLoader.GetMaterial(iSubset);
			pMesh->DrawSubset(iSubset);
		}
	});
}

void CScene::Destroy()
{
	Msg("Destroying scene...");

	m_MeshLoader.Destroy();

	SetSceneLoaded(false);
}
///////////////////////////////////////////////////////////////
