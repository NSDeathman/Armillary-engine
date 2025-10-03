///////////////////////////////////////////////////////////////
// Created: 20.09.2025
// Author: NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "../Core/application_interface.h"
#include "../Core/IGame.h"
#include "../Core/Render.h"
///////////////////////////////////////////////////////////////
using namespace Core;
using namespace Core::Math;
using namespace Core::Debug;
using namespace Render;
///////////////////////////////////////////////////////////////
class CEngine : public CApplicationBase
{
private:
	HMODULE m_GameModule = nullptr;
	std::unique_ptr<IGame> m_Game;

	Render::CRender m_Render;
	IRenderBackend* m_RenderBackend = m_Render.GetBackend();

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
