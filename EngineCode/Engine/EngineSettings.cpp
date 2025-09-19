///////////////////////////////////////////////////////////////
// Created: 28.08.2025
// Author: NS_Deathman
// Game settings realization
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EngineSettings.h"
#include "filesystem.h"
#include "log.h"
///////////////////////////////////////////////////////////////
EngineSettings::EngineSettings()
{
	Msg("Loading engine settings config file with name: %s", ENGINE_SETTINGS_CONFIG_NAME.c_str());

	m_Config = new (IniParser);

	bool ConfigFileExist = false;

	ConfigFileExist = m_Config->Load(ENGINE_SETTINGS_CONFIG_NAME, APPLICATION_DATA);

	if (!ConfigFileExist)
	{
		Msg("Can't find engine settings config file, creating new");
		Save();
	}
}

void EngineSettings::Save()
{
	Msg("Saving engine settings config file");

	m_Config->Save(ENGINE_SETTINGS_CONFIG_NAME, APPLICATION_DATA);
}

EngineSettings::~EngineSettings()
{
	Msg("Destroying engine config");

	Save();

	delete (m_Config);
}
///////////////////////////////////////////////////////////////
EngineSettings* Settings = nullptr;
///////////////////////////////////////////////////////////////
