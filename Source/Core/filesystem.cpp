///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: DeepSeek, NS_Deathman
// Filesystem implementation
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "filesystem.h"
#include "log.h"
///////////////////////////////////////////////////////////////
namespace Core
{
	void CFilesystem::Initialize()
	{
		if (m_Initialized)
		{
			Print("Filesystem already initialized!");
			return;
		}

		try
		{
			char path[MAX_PATH];
			GetModuleFileNameA(NULL, path, MAX_PATH);

			fs::path exeDir = fs::path(path).parent_path();

			m_ExecutableDir = exeDir.parent_path();

			m_WorkingDir = fs::current_path();

			Print("Filesystem initialized successfully");
			Print("Executable dir: %s", m_ExecutableDir.string().c_str());
			Print("Working dir: %s \n", m_WorkingDir.string().c_str());

			m_Initialized = true;
		}
		catch (const std::exception& e)
		{
			Print("Failed to initialize filesystem: %s", e.what());
		}
	}

	void CFilesystem::Destroy()
	{
		if (m_Initialized)
		{
			Print("Destroying filesystem...");
			m_Initialized = false;
		}
	}

	fs::path CFilesystem::GetPath(const std::string& first, const std::initializer_list<std::string>& parts)
	{
		fs::path result = first;
		for (const auto& part : parts)
		{
			result /= part;
		}
		return result;
	}

	fs::path CFilesystem::GetAppDataPath(const std::initializer_list<std::string>& parts)
	{
		fs::path result = m_ExecutableDir / "appdata";
		for (const auto& part : parts)
		{
			result /= part;
		}
		return result;
	}

	fs::path CFilesystem::GetGameResourcesPath(const std::initializer_list<std::string>& parts)
	{
		fs::path result = m_ExecutableDir / "gameresources";
		for (const auto& part : parts)
		{
			result /= part;
		}
		return result;
	}

	bool CFilesystem::CreateDirectoryRecursive(const fs::path& path, std::error_code& err)
	{
		err.clear();
		if (!fs::create_directories(path, err))
		{
			if (fs::exists(path))
			{
				err.clear();
				return true;
			}
			return false;
		}
		return true;
	}

	bool CFilesystem::CreateDirectory(const fs::path& path)
	{
		std::error_code err;
		if (!CreateDirectoryRecursive(path, err))
		{
			Print("CreateDirectory FAILED: %s, error: %s", path.string().c_str(), err.message().c_str());
			return false;
		}
		return true;
	}

	bool CFilesystem::FileExists(const fs::path& path)
	{
		return fs::exists(path) && fs::is_regular_file(path);
	}

	bool CFilesystem::DirectoryExists(const fs::path& path)
	{
		return fs::exists(path) && fs::is_directory(path);
	}

	std::vector<fs::path> CFilesystem::GetFiles(const fs::path& directory, const std::string& extension)
	{
		std::vector<fs::path> files;

		if (!DirectoryExists(directory))
			return files;

		try
		{
			for (const auto& entry : fs::directory_iterator(directory))
			{
				if (entry.is_regular_file())
				{
					if (extension.empty() || entry.path().extension() == extension)
					{
						files.push_back(entry.path());
					}
				}
			}
		}
		catch (const std::exception& e)
		{
			Print("GetFiles failed: %s", e.what());
		}

		return files;
	}

	std::vector<fs::path> CFilesystem::GetDirectories(const fs::path& directory)
	{
		std::vector<fs::path> directories;

		if (!DirectoryExists(directory))
			return directories;

		try
		{
			for (const auto& entry : fs::directory_iterator(directory))
			{
				if (entry.is_directory())
				{
					directories.push_back(entry.path());
				}
			}
		}
		catch (const std::exception& e)
		{
			Print("GetDirectories failed: %s", e.what());
		}

		return directories;
	}

	std::string CFilesystem::ReadTextFile(const fs::path& path)
	{
		if (!FileExists(path))
		{
			Print("File not found: %s", path.string().c_str());
			return "";
		}

		try
		{
			std::ifstream file(path);
			if (!file.is_open())
			{
				throw std::runtime_error("Failed to open file");
			}

			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			return content;
		}
		catch (const std::exception& e)
		{
			Print("ReadTextFile failed: %s - %s", path.string().c_str(), e.what());
			return "";
		}
	}

	bool CFilesystem::WriteTextFile(const fs::path& path, const std::string& content)
	{
		try
		{
			// Создаем директорию если нужно
			CreateDirectory(path.parent_path());

			std::ofstream file(path);
			if (!file.is_open())
			{
				throw std::runtime_error("Failed to open file for writing");
			}

			file << content;
			return true;
		}
		catch (const std::exception& e)
		{
			Print("WriteTextFile failed: %s - %s", path.string().c_str(), e.what());
			return false;
		}
	}

	void CFilesystem::SetWorkingDir(const fs::path& path)
	{
		if (DirectoryExists(path))
		{
			fs::current_path(path);
			m_WorkingDir = path;
			Print("Working directory changed to: %s", path.string().c_str());
		}
		else
		{
			Print("Directory does not exist: %s", path.string().c_str());
		}
	}
///////////////////////////////////////////////////////////////
} // namespace Core
///////////////////////////////////////////////////////////////