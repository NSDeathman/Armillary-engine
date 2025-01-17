///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Scene class
///////////////////////////////////////////////////////////////
#include "scene.h"
#include "render.h"
#include "log.h"
///////////////////////////////////////////////////////////////
void CScene::Load()
{
	Msg("Loading scene...");

	m_MeshLoader.Create(Device, "cup.obj");

	Msg("Scene loaded successfully");

	SetSceneLoaded(true);
}

void CScene::DrawGeometry()
{
	// Meshes are divided into subsets, one for each material. Render them in a loop
	concurrency::parallel_for(UINT(0), m_MeshLoader.GetNumMaterials(), [this](UINT iSubset) 
	{
		ID3DXMesh* pMesh = m_MeshLoader.GetMesh();
		Material* pMaterial = m_MeshLoader.GetMaterial(iSubset);
		pMesh->DrawSubset(iSubset);
	});
}

void CScene::Destroy()
{
	Msg("Destroying scene...");

	m_MeshLoader.Destroy();

	SetSceneLoaded(false);
}
///////////////////////////////////////////////////////////////
