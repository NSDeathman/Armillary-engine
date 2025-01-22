///////////////////////////////////////////////////////////////
// Created: 18.01.2025
// Author: NS_Deathman
// User interface class
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "log.h"
///////////////////////////////////////////////////////////////
class CUserInterface
{
  private:
	bool m_bNeedLoadScene;
	bool m_bNeedDestroyScene;
	bool m_bHelperWndDraw;

  public:
	CUserInterface() = default;
	~CUserInterface() = default;

	void Initialize();
	void UpdateIngameUI();
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

	void SetNeedLoadScene(bool flag)
	{
		m_bNeedLoadScene = flag;
	}

	bool NeedDestroyScene()
	{
		return m_bNeedDestroyScene;
	}

	void SetNeedDestroyScene(bool flag)
	{
		m_bNeedDestroyScene = flag;
	}
};
///////////////////////////////////////////////////////////////
extern CUserInterface* UserInterface;
///////////////////////////////////////////////////////////////
