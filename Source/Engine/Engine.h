///////////////////////////////////////////////////////////////
// Created: 20.09.2025
// Author: NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "../Core/application_interface.h"
#include "../Game/Game.h"
///////////////////////////////////////////////////////////////
class CEngine : public Core::CApplicationBase
{
private:
	HMODULE m_GameModule = nullptr;
	std::unique_ptr<IGame> m_Game;

private:
	bool g_bNeedCloseApplication;

	void Start() override;
	void Destroy() override;

	void Close(){};
	void Pause(){};

	void HandleSDLEvents();
	void Update();

	bool LoadGameModule();
	void LoadRender();

public:
	void Process() override;
};
///////////////////////////////////////////////////////////////
