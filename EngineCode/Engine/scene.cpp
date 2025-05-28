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
	m_bSceneLoaded = false;
}

struct ModData
{
	bool X;
	bool Y;
	bool Z;
	float Mod;
};

ModData SpeedModifiers[8] = 
{	
	ModData{false, true, false, 0.75f},
	ModData{false, true, false, 0.5f},
	ModData{false, true, false, -1.25f},
	ModData{false, true, false, -0.75f},
	ModData{true, false, false, 0.8f},
	ModData{false, true, false, -0.8f},
	ModData{false, true, false, 0.25f},
	ModData{false, true, false, 0.9f}
};

void LoadMeshes()
{
	Scene->SetSceneLoadingState(true);

	Scene->SceneMeshesArray[0].Create(Device, MESHES, "core.obj");
	Scene->SceneMeshesArray[1].Create(Device, MESHES, "armor.obj");
	Scene->SceneMeshesArray[2].Create(Device, MESHES, "ring_small_0.obj");
	Scene->SceneMeshesArray[3].Create(Device, MESHES, "ring_small_1.obj");
	Scene->SceneMeshesArray[4].Create(Device, MESHES, "ring_small_2.obj");
	Scene->SceneMeshesArray[5].Create(Device, MESHES, "ring_big_0.obj");
	Scene->SceneMeshesArray[6].Create(Device, MESHES, "ring_big_1.obj");
	Scene->SceneMeshesArray[7].Create(Device, MESHES, "ring_big_2.obj");

	Scene->SetSceneLoadingState(false);
	Scene->SetSceneLoaded(true);

	Msg("Scene loaded successfully");
}

void CScene::DestroyMeshes()
{
	for (int i = 0; i < 8; i++)
		SceneMeshesArray[i].Destroy();

	Scene->SetSceneLoaded(false);

	Msg("Scene destroyed successfully");
}

void CScene::Load()
{
	Msg("Loading scene...");

	Scheduler->Add(LoadMeshes);
}

void CScene::SetWorldMatrix()
{
	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
	Render->m_pDirect3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

void CScene::DrawMeshSubsets(CMeshLoader* Mesh)
{
	// Meshes are divided into subsets, one for each material. Render them in a loop
	for (int iSubset = 0; iSubset < Mesh->GetNumMaterials(); iSubset++)
	{
		ID3DXMesh* pMesh = Mesh->GetMesh();
		if (pMesh)
		{
			Material* pMaterial = Mesh->GetMaterial(iSubset);
			pMesh->DrawSubset(iSubset);
		}
	}
}

void CScene::DrawGeometry()
{
	for (int MeshesIterator = 0; MeshesIterator < 8; MeshesIterator++)
	{
		ModData Mod = SpeedModifiers[MeshesIterator];

		float SpeedModifier = Mod.Mod;

		D3DXMATRIXA16 matWorld;
		float Timer = timeGetTime() / 1000.0f;
		float AnglePerFrame = Timer * SpeedModifier;

		if (Mod.X)
			D3DXMatrixRotationX(&matWorld, AnglePerFrame);
		else if (Mod.Y)
			D3DXMatrixRotationY(&matWorld, AnglePerFrame);
		else if (Mod.Z)
			D3DXMatrixRotationZ(&matWorld, AnglePerFrame);

		Render->m_pDirect3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		DrawMeshSubsets(&SceneMeshesArray[MeshesIterator]);
	}
}

void CScene::Destroy()
{
	Msg("Destroying scene...");

	DestroyMeshes();
}
///////////////////////////////////////////////////////////////
