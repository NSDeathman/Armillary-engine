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
///////////////////////////////////////////////////////////////
class IniParser
{
  public:
	bool Load(const string& filename, const string path = "");
	bool Save(const string& filename, const string path = "");

	string GetString(const string& section, const string& key, const string& defaultValue = "");

	int GetInt(const string& section, const string& key, int defaultValue = 0);

	float GetFloat(const string& section, const string& key, float defaultValue = 0.0f);

	bool GetBool(const string& section, const string& key, bool defaultValue = false);

	void SetString(const string& section, const string& key, const string& value);

	void SetInt(const string& section, const string& key, int value);

	void SetFloat(const string& section, const string& key, float value);

	void SetBool(const string& section, const string& key, bool value);

	bool HasSection(const string& section) const;
	bool HasKey(const string& section, const string& key) const;

	std::vector<string> GetSections() const;
	std::vector<string> GetKeys(const string& section) const;

  private:
	std::unordered_map<string, std::unordered_map<string, string>> m_Data;

	string Trim(const string& str);
	bool IsSection(const string& line);
	bool IsKeyValue(const string& line);
};
///////////////////////////////////////////////////////////////
