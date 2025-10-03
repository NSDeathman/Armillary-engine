///////////////////////////////////////////////////////////////
// Created: 28.08.2025
// Author: DeepSeek, NS_Deathman
// Ini config realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <string>
#include <unordered_map>
#include <vector>
#include "MathAPI.h"
///////////////////////////////////////////////////////////////
class IniParser
{
  public:
	bool Load(const std::string& filename, const std::string path = "");
	bool Save(const std::string& filename, const std::string path = "");

	std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue = "");

	int GetInt(const std::string& section, const std::string& key, int defaultValue = 0);

	float GetFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f);

	bool GetBool(const std::string& section, const std::string& key, bool defaultValue = false);

	void SetString(const std::string& section, const std::string& key, const std::string& value);

	void SetInt(const std::string& section, const std::string& key, int value);

	void SetFloat(const std::string& section, const std::string& key, float value);

	void SetBool(const std::string& section, const std::string& key, bool value);

	bool HasSection(const std::string& section) const;
	bool HasKey(const std::string& section, const std::string& key) const;

	std::vector<std::string> GetSections() const;
	std::vector<std::string> GetKeys(const std::string& section) const;

  private:
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_Data;

	std::string Trim(const std::string& str);
	bool IsSection(const std::string& line);
	bool IsKeyValue(const std::string& line);
};
///////////////////////////////////////////////////////////////
