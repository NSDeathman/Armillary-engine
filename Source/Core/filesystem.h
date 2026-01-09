///////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: DeepSeek, NS_Deathman
// Filesystem implementation
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Core.h"
#include <filesystem>
///////////////////////////////////////////////////////////////
#define APPLICATION_DATA "..//appdata//"
#define DEBUG_DATA "..//debugdata//"
#define GAME_RESOURCES "..//gameresources//"
#define LOGS "..//appdata//logs//"
#define TEXTURES "..//gameresources//textures//"
#define MESHES "..//gameresources//meshes//"
#define CONFIGS "..//gameresources//configs//"
#define SHADERS "..//gameresources//shaders//"
#define SOUNDS "..//gameresources//sounds//"
#define FONTS "..//gameresources//fonts//"
///////////////////////////////////////////////////////////////
namespace fs = std::filesystem;
///////////////////////////////////////////////////////////////
namespace Core
{
	class  CFilesystem
	{
	  public:
		// Получение экземпляра синглтона
		static CFilesystem& GetInstance();

		CFilesystem(const CFilesystem&) = delete;
		CFilesystem& operator=(const CFilesystem&) = delete;

		void Initialize();
		void Destroy();

		fs::path GetPath(const std::string& first, const std::initializer_list<std::string>& parts = {});
		fs::path GetAppDataPath(const std::initializer_list<std::string>& parts = {});
		fs::path GetGameResourcesPath(const std::initializer_list<std::string>& parts = {});

		bool CreateDirectory(const fs::path& path);
		bool FileExists(const fs::path& path);
		bool DirectoryExists(const fs::path& path);

		std::vector<fs::path> GetFiles(const fs::path& directory, const std::string& extension = "");
		std::vector<fs::path> GetDirectories(const fs::path& directory);

		std::string ReadTextFile(const fs::path& path);
		bool WriteTextFile(const fs::path& path, const std::string& content);

		fs::path GetExecutableDir() const
		{
			return m_ExecutableDir;
		}
		fs::path GetWorkingDir() const
		{
			return m_WorkingDir;
		}
		void SetWorkingDir(const fs::path& path);

		bool IsInitialized() const
		{
			return m_Initialized;
		}

	  private:
		CFilesystem();
		~CFilesystem();

		bool m_Initialized = false;
		fs::path m_ExecutableDir;
		fs::path m_WorkingDir;

		bool CreateDirectoryRecursive(const fs::path& path, std::error_code& err);
	};
} // namespace Core
///////////////////////////////////////////////////////////////
// Макросы для удобного использования
#define FS Core::CFilesystem::GetInstance()
#define FS_INIT() FS.Initialize()
#define FS_DESTROY() FS.Destroy()
///////////////////////////////////////////////////////////////
