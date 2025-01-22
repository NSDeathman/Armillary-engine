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
#define APPLICATION_DATA "..\\appdata\\"
#define LOGS "..\\appdata\\logs\\"
#define GAME_RESOURCES "..\\gameresources\\"
#define TEXTURES "..//gameresources//textures\//"
#define MESHES "..//GameResources//meshes//"
#define SHADERS "..\\gameresources\\shaders\\"
#define SOUNDS "..\\gameresources\\sounds\\"
#define FONTS "..\\gameresources\\fonts\\"
///////////////////////////////////////////////////////////////
namespace fs = std::filesystem;
///////////////////////////////////////////////////////////////
class CFilesystem
{
private:
	bool CreateDirectoryRecursive(std::string const& dirName, std::error_code& err);

public:
	void CreateDir(std::string const& dirName);
	void Destroy();

	CFilesystem() = default;
	~CFilesystem() = default;
};
///////////////////////////////////////////////////////////////
/*
TCHAR GetAbsolutePath(TCHAR file_path)
{
	TCHAR* fileExt;
	TCHAR filename[256];

	GetFullPathName(file_path, 256, filename, &fileExt);

	return filename;
}
*/
///////////////////////////////////////////////////////////////
extern CFilesystem* Filesystem;
///////////////////////////////////////////////////////////////
#endif //FILESYSTEM_INCLUDED
///////////////////////////////////////////////////////////////
