///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Scene class
///////////////////////////////////////////////////////////////
#include "scene.h"
#include "log.h"
#include "render_DX9.h"
#include "render_backend_DX9.h"
#include "IniParser.h"
#include "Monitoring.h"
#include "AsyncExecutor.h"
#include "user_interface.h"
///////////////////////////////////////////////////////////////
CScene::CScene()
{
	m_bSceneLoadingInProcess = false;
}

void CScene::Load()
{
	Msg("Loading scene...");

	AsyncExecutor->Execute([]() 
	{
		Monitoring->TaskMonitor.addTask("LoadMesh()");

		Scene->SetSceneLoadingState(true);

		Scene->m_Mesh.Create(MESHES, "debug_plane.obj");

		Scene->SetSceneLoadingState(false);
		Scene->SetSceneLoaded(true);

		Msg("Scene loaded successfully");

		Monitoring->TaskMonitor.completeTask("LoadMesh()");
	});
}

void CScene::OnFrame()
{
	if (UserInterface->NeedLoadScene())
		Load();

	if (UserInterface->NeedDestroyScene())
	{
		Destroy();
		UserInterface->SetNeedDestroyScene(false);
	}
}

void CScene::DrawGeometry()
{
	Scene->m_Mesh.DrawSubsets();
}

void DestroyMesh()
{
	Scene->m_Mesh.Destroy();

	Scene->SetSceneLoaded(false);

	Msg("Scene destroyed successfully");
}

void CScene::Destroy()
{
	Msg("Destroying scene...");

	DestroyMesh();
}
///////////////////////////////////////////////////////////////
CScene* Scene = nullptr;
///////////////////////////////////////////////////////////////
