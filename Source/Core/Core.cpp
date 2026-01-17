///////////////////////////////////////////////////////////////
// Created: 19.09.2025
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Core.h"
///////////////////////////////////////////////////////////////
using namespace Core;
using namespace Core::Debug;
///////////////////////////////////////////////////////////////
void CCoreAPI::Initialize()
{
	Logger.Initialize(std::string(LOGS));

	Print("Initializing CoreAPI");

	ErrorHandler::initialize();

	InitializeCPU();

	PrintBuildData();

	Filesystem.Initialize();

	TimeSystem.Initialize();

	Input.Initialize();

	Print("CoreAPI initialized");
}

void CCoreAPI::Destroy()
{
	Print("Destroying CoreAPI");

	Input.Destroy();
	TimeSystem.Destroy();
	Filesystem.Destroy();
	Logger.Destroy();
}

CCoreAPI::~CCoreAPI()
{
	Destroy();
}
