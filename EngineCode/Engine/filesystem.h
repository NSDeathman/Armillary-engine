///////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: NS_Deathman
// Filesystem implementation
///////////////////////////////////////////////////////////////
#ifndef FILESYSTEM_INCLUDED
#define FILESYSTEM_INCLUDED
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <filesystem>
///////////////////////////////////////////////////////////////
#define APPLICATION_DATA "..//appdata//"
#define GAME_RESOURCES "..//gameresources//"
#define LOGS "..//appdata//logs//"
#define TEXTURES "..//gameresources//textures//"
#define MESHES "..//GameResources//meshes//"
#define CONFIGS "..//GameResources//configs//"
#define SHADERS "..//gameresources//shaders//"
#define SOUNDS "..//gameresources//sounds//"
#define FONTS "..//gameresources//fonts//"
///////////////////////////////////////////////////////////////
class CFilesystem
{
private:
	bool CreateDirectoryRecursive(string const& dirName, std::error_code& err);

public:
	void CreateDir(string const& dirName);
	string GetExecutableFilePath();
	string GetAbsolutePath(string file_path, string file_name);
	void Destroy();

	CFilesystem() = default;
	~CFilesystem() = default;
};
///////////////////////////////////////////////////////////////
extern CFilesystem* Filesystem;
///////////////////////////////////////////////////////////////
#endif //FILESYSTEM_INCLUDED
///////////////////////////////////////////////////////////////
