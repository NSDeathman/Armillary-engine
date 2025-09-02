///////////////////////////////////////////////////////////////
// Created: 28.08.2025
// Author: DeepSeek, NS_Deathman
// Ini config update watcher realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "IniParser.h"
#include <ctime>
#include "stdafx.h"
///////////////////////////////////////////////////////////////
class ConfigWatcher
{
  public:
	ConfigWatcher(IniParser& config, const string& filename): m_Config(config), m_Filename(filename), m_LastModTime(0)
	{
	}

	bool CheckForChanges()
	{
		auto modTime = GetLastModificationTime();
		if (modTime > m_LastModTime) [[unlikely]]
		{
			m_LastModTime = modTime;
			return m_Config.Load(m_Filename);
		}
		return false;
	}

  private:
	IniParser& m_Config;
	std::string m_Filename;
	std::time_t m_LastModTime;

	std::time_t GetLastModificationTime()
	{
		struct stat result;
		if (stat(m_Filename.c_str(), &result) == 0)
		{
			return result.st_mtime;
		}
		return 0;
	}
};
///////////////////////////////////////////////////////////////
