///////////////////////////////////////////////////////////////
// Game.h
///////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include "../Core/IGame.h"

#include "../Core/Scene.h"
#include "../Core/Camera.h"
#include "../Core/CameraController.h"
#include "../Core/FlyingCameraController.h"

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
	std::shared_ptr<Core::World::ICameraController> m_CameraController;
};

extern IGame* CreateGame();
