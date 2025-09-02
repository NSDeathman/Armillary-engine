///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Scene class
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "MeshLoader.h"
///////////////////////////////////////////////////////////////
class CScene
{
  private:
	bool m_bSceneLoaded;
	bool m_bSceneLoadingInProcess;

  public:
	CMesh m_Mesh;

	CScene();
	~CScene() = default;

	void Load();
	void DrawGeometry();
	void Destroy();

	void SetSceneLoaded(bool flag)
	{
		m_bSceneLoaded = flag;
	}

	bool Ready()
	{
		return m_bSceneLoaded;
	}

	void SetSceneLoadingState(bool state)
	{
		m_bSceneLoadingInProcess = state;
	}

	bool isLoading()
	{
		return m_bSceneLoadingInProcess;
	}
};
///////////////////////////////////////////////////////////////
extern CScene* Scene;
///////////////////////////////////////////////////////////////
