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
#include "render_backend_DX9.h"
#endif

#include "IniParser.h"
///////////////////////////////////////////////////////////////
CScene::CScene()
{
	m_bSceneLoadingInProcess = false;
}

void LoadMesh()
{
	Scene->SetSceneLoadingState(true);

	Scene->m_Mesh.Create(MESHES, "debug_plane.obj");

	Scene->SetSceneLoadingState(false);
	Scene->SetSceneLoaded(true);

	Msg("Scene loaded successfully");
}

void DestroyMesh()
{
	Scene->m_Mesh.Destroy();

	Scene->SetSceneLoaded(false);

	Msg("Scene destroyed successfully");
}

void CScene::Load()
{
	Msg("Loading scene...");

	Scheduler->Add(LoadMesh);

	/*
	IniParser* Config = new (IniParser);
	Config->SetFloat(string("test_section"), string("test_float"), 1.79f);
	Config->Save(string("test_config.ini"), CONFIGS);

	Config->Load(string("test_config.ini"), CONFIGS);
	float Value = Config->GetFloat(string("test_section"), string("test_float"));
	Msg("Value = %f", Value);
	delete (Config);
	*/
}

void CScene::DrawGeometry()
{
	Scene->m_Mesh.DrawSubsets();
}

void CScene::Destroy()
{
	Msg("Destroying scene...");

	DestroyMesh();
}
///////////////////////////////////////////////////////////////
