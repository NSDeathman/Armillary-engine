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
using namespace Core;
using namespace Math;
///////////////////////////////////////////////////////////////
bool IniParser::Load(const std::string& filename, const std::string path)
{
	std::ifstream file(path + filename);
	if (!file.is_open())
	{
		Print("Can't load config file with name %s", filename.c_str());
		return false;
	}

	m_Data.clear();
	std::string currentSection;
	std::string line;

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
			std::string key = Trim(line.substr(0, equalsPos));
			std::string value = Trim(line.substr(equalsPos + 1));

			m_Data[currentSection][key] = value;
		}
	}

	file.close();
	return true;
}

bool IniParser::Save(const std::string& filename, const std::string path)
{
	std::ofstream file(path + filename);
	if (!file.is_open())
	{
		Print("Can't save config file with name %s", filename.c_str());
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

std::string IniParser::GetString(const std::string& section, const std::string& key, const std::string& defaultValue)
{
	if (m_Data.find(section) != m_Data.end())
	{
		const auto& sectionData = m_Data[section];
		if (sectionData.find(key) != sectionData.end())
		{
			return sectionData.at(key);
		}
	}

	Print("Can't find std::string key %s in section with name %s", key.c_str(), section.c_str());
	return defaultValue;
}

int IniParser::GetInt(const std::string& section, const std::string& key, int defaultValue)
{
	std::string value = GetString(section, key);
	if (value.empty())
	{
		Print("Can't find int key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}

	try
	{
		return std::stoi(value);
	}
	catch (...)
	{
		Print("Can't convert int key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}
}

float IniParser::GetFloat(const std::string& section, const std::string& key, float defaultValue)
{
	std::string value = GetString(section, key);
	if (value.empty())
	{
		Print("Can't find float key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}

	try
	{
		return std::stof(value);
	}
	catch (...)
	{
		Print("Can't convert float key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}
}

bool IniParser::GetBool(const std::string& section, const std::string& key, bool defaultValue)
{
	std::string value = GetString(section, key);
	if (value.empty())
	{
		Print("Can't find bool key %s in section with name %s", key.c_str(), section.c_str());
		return defaultValue;
	}

	std::string lowerValue = value;
	//std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);

	if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes")
	{
		return true;
	}
	else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no")
	{
		return false;
	}

	Print("Can't get bool key %s in section with name %s", key.c_str(), section.c_str());
	return defaultValue;
}

void IniParser::SetString(const std::string& section, const std::string& key, const std::string& value)
{
	m_Data[section][key] = value;
}

void IniParser::SetInt(const std::string& section, const std::string& key, int value)
{
	m_Data[section][key] = std::to_string(value);
}

void IniParser::SetFloat(const std::string& section, const std::string& key, float value)
{
	m_Data[section][key] = std::to_string(value);
}

void IniParser::SetBool(const std::string& section, const std::string& key, bool value)
{
	m_Data[section][key] = value ? "true" : "false";
}

bool IniParser::HasSection(const std::string& section) const
{
	return m_Data.find(section) != m_Data.end();
}

bool IniParser::HasKey(const std::string& section, const std::string& key) const
{
	auto it = m_Data.find(section);
	if (it != m_Data.end())
	{
		return it->second.find(key) != it->second.end();
	}
	return false;
}

std::vector<std::string> IniParser::GetSections() const
{
	std::vector<std::string> sections;
	for (const auto& pair : m_Data)
	{
		sections.push_back(pair.first);
	}
	return sections;
}

std::vector<std::string> IniParser::GetKeys(const std::string& section) const
{
	std::vector<std::string> keys;
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

std::string IniParser::Trim(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");

	if (start == std::string::npos || end == std::string::npos)
	{
		return "";
	}

	return str.substr(start, end - start + 1);
}

bool IniParser::IsSection(const std::string& line)
{
	return line.size() >= 2 && line[0] == '[' && line[line.size() - 1] == ']';
}

bool IniParser::IsKeyValue(const std::string& line)
{
	return line.find('=') != std::string::npos;
}
///////////////////////////////////////////////////////////////
