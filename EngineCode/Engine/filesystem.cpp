///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Filesystem implementation
///////////////////////////////////////////////////////////////
#include "filesystem.h"
#include "log.h"
///////////////////////////////////////////////////////////////
namespace fs = std::filesystem;
///////////////////////////////////////////////////////////////
bool CFilesystem::CreateDirectoryRecursive(string const& dirName, std::error_code& err)
{
	err.clear();
	if (!fs::create_directories(dirName, err))
	{
		if (fs::exists(dirName))
		{
			// The folder already exists:
			err.clear();
			return true;
		}
		return false;
	}
	return true;
}

void CFilesystem::CreateDir(string const& dirName)
{
	std::error_code err;
	if (!CreateDirectoryRecursive(dirName, err))
	{
		// Report the error:
		std::cout << "CreateDirectoryRecursive FAILED, err: " << err.message() << std::endl;
	}
}

std::string CFilesystem::GetExecutableFilePath()
{
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	return std::string(path);
}

string CFilesystem::GetAbsolutePath(string FilePath, string FileName)
{
	fs::path ExePath = GetExecutableFilePath();
	fs::path ExeDir = ExePath.parent_path();

	fs::path TargetFile = ExeDir / (FilePath + FileName);
	fs::path CanonicalPath = fs::canonical(TargetFile);

	return CanonicalPath.string();
}

void CFilesystem::Destroy()
{
	Msg("Destroying filesystem...");
}
///////////////////////////////////////////////////////////////
CFilesystem* Filesystem = nullptr;
///////////////////////////////////////////////////////////////
