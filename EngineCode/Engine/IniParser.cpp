///////////////////////////////////////////////////////////////
// Created: 28.08.2025
// Author: DeepSeek, NS_Deathman
// Ini config realization
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "IniParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "log.h"
///////////////////////////////////////////////////////////////
bool IniParser::Load(const string& filename, const string path)
{
	std::ifstream file(path + filename);
	if (!file.is_open())
	{
		Msg("Can't load config file with name %s", filename.c_str());
		return false;
	}

	m_Data.clear();
	string currentSection;
	string line;

	while (std::getline(file, line))
	{
		line = Trim(line);

		// Пропускаем пустые строки и комментарии
		if (line.empty() || line[0] == ';' || line[0] == '#')
		{
			continue;
		}

		if (IsSection(line))
		{
			// Обработка секции [SectionName]
			currentSection = line.substr(1, line.size() - 2);
			currentSection = Trim(currentSection);
		}
		else if (IsKeyValue(line))
		{
			// Обработка ключ=значение
			size_t equalsPos = line.find('=');
			string key = Trim(line.substr(0, equalsPos));
			string value = Trim(line.substr(equalsPos + 1));

			m_Data[currentSection][key] = value;
		}
	}

	file.close();
	return true;
}

bool IniParser::Save(const string& filename, const string path)
{
	std::ofstream file(path + filename);
	if (!file.is_open())
	{
		Msg("Can't save config file with name %s", filename.c_str());
		return false;
	}

	for (const auto& sectionPair : m_Data)
	{
		if (!sectionPair.first.empty())
		{
			file << "[" << sectionPair.first << "]\n";
		}

		for (const auto& keyValuePair : sectionPair.second)
		{
			file << keyValuePair.first << " = " << keyValuePair.second << "\n";
		}

		file << "\n";
	}

	file.close();
	return true;
}

string IniParser::GetString(const string& section, const string& key, const string& defaultValue)
{
	if (m_Data.find(section) != m_Data.end())
	{
		const auto& sectionData = m_Data[section];
		if (sectionData.find(key) != sectionData.end())
		{
			return sectionData.at(key);
		}
	}

	Msg("Can't find string key %s in section with name %s", key.c_str(), section.c_str());
	return defaultValue;
}

int IniParser::GetInt(const string& section, const string& key, int defaultValue)
{
	string value = GetString(section, key);
	if (value.empty())
	{
		Msg("Can't find int key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}

	try
	{
		return std::stoi(value);
	}
	catch (...)
	{
		Msg("Can't convert int key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}
}

float IniParser::GetFloat(const string& section, const string& key, float defaultValue)
{
	string value = GetString(section, key);
	if (value.empty())
	{
		Msg("Can't find float key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}

	try
	{
		return std::stof(value);
	}
	catch (...)
	{
		Msg("Can't convert float key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}
}

bool IniParser::GetBool(const string& section, const string& key, bool defaultValue)
{
	string value = GetString(section, key);
	if (value.empty())
	{
		Msg("Can't find bool key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}

	string lowerValue = value;
	//std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);

	if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes")
	{
		return true;
	}
	else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no")
	{
		return false;
	}

	Msg("Can't get bool key %s in section with name %s", key.c_str(), section.c_str());
	return defaultValue;
}

void IniParser::SetString(const string& section, const string& key, const string& value)
{
	m_Data[section][key] = value;
}

void IniParser::SetInt(const string& section, const string& key, int value)
{
	m_Data[section][key] = std::to_string(value);
}

void IniParser::SetFloat(const string& section, const string& key, float value)
{
	m_Data[section][key] = std::to_string(value);
}

void IniParser::SetBool(const string& section, const string& key, bool value)
{
	m_Data[section][key] = value ? "true" : "false";
}

bool IniParser::HasSection(const string& section) const
{
	return m_Data.find(section) != m_Data.end();
}

bool IniParser::HasKey(const string& section, const string& key) const
{
	auto it = m_Data.find(section);
	if (it != m_Data.end())
	{
		return it->second.find(key) != it->second.end();
	}
	return false;
}

std::vector<string> IniParser::GetSections() const
{
	std::vector<string> sections;
	for (const auto& pair : m_Data)
	{
		sections.push_back(pair.first);
	}
	return sections;
}

std::vector<string> IniParser::GetKeys(const string& section) const
{
	std::vector<string> keys;
	auto it = m_Data.find(section);
	if (it != m_Data.end())
	{
		for (const auto& pair : it->second)
		{
			keys.push_back(pair.first);
		}
	}
	return keys;
}

string IniParser::Trim(const string& str)
{
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");

	if (start == string::npos || end == string::npos)
	{
		return "";
	}

	return str.substr(start, end - start + 1);
}

bool IniParser::IsSection(const string& line)
{
	return line.size() >= 2 && line[0] == '[' && line[line.size() - 1] == ']';
}

bool IniParser::IsKeyValue(const string& line)
{
	return line.find('=') != string::npos;
}
///////////////////////////////////////////////////////////////
