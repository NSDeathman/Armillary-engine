///////////////////////////////////////////////////////////////
// Game.h
///////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include <IGame.h>
#include <Scene.h>

class CGame : public IGame
{
  public:
	virtual ~CGame() = default;

	bool Initialize() override;
	void Shutdown() override;
	void Update() override;

  private:
	// Главная сцена, которая хранит объекты и управляет рендером
	std::shared_ptr<Core::World::CScene> m_Scene;

	// Камера и её контроллер
	std::shared_ptr<Core::World::CCamera> m_MainCamera;
};

extern IGame* CreateGame();
