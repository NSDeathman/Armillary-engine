///////////////////////////////////////////////////////////////
// Created: 17.01.2025
// Author: NS_Deathman
// Filesystem implementation
///////////////////////////////////////////////////////////////
#include "filesystem.h"
#include "log.h"
///////////////////////////////////////////////////////////////
bool CFilesystem::CreateDirectoryRecursive(std::string const& dirName, std::error_code& err)
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

void CFilesystem::CreateDir(std::string const& dirName)
{
	std::error_code err;
	if (!CreateDirectoryRecursive(LOGS, err))
	{
		// Report the error:
		std::cout << "CreateDirectoryRecursive FAILED, err: " << err.message() << std::endl;
	}
}

void CFilesystem::Destroy()
{
	Msg("Destroying filesystem...");
}
///////////////////////////////////////////////////////////////
