///////////////////////////////////////////////////////////////
// Created: 18.01.2025
// Author: NS_Deathman
// User interface class
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////
class CUserInterface
{
  private:
	bool m_bNeedDraw;
	bool m_bNeedLoadScene;
	bool m_bNeedDestroyScene;

  public:
	CUserInterface() = default;
	~CUserInterface() = default;

	void Initialize();
	void OnFrameBegin();
	void OnFrame();
	void OnFrameEnd();
	void Render();
	void OnResetBegin();
	void OnResetEnd();
	void Destroy();

	bool NeedLoadScene()
	{
		return m_bNeedLoadScene;
	}

	bool NeedDestroyScene()
	{
		return m_bNeedDestroyScene;
	}

	void SceneDestroyed()
	{
		m_bNeedDestroyScene = false;
	}
};
///////////////////////////////////////////////////////////////
extern CUserInterface* UserInterface;
///////////////////////////////////////////////////////////////
