///////////////////////////////////////////////////////////////
//Created: 15.01.2025
//Author: NS_Deathman
//Log realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <string>
#include <fstream>
#include "filesystem.h"
///////////////////////////////////////////////////////////////
class CLog
{
private:
	std::ofstream m_LogFileStream;

public:
	void CreateConsole();
	void __cdecl Print(LPCSTR format, ...);
	void __cdecl Debug_Print(LPCSTR format, ...);

	void Flush();
	void Destroy();

	CLog();
	~CLog() = default;
};
///////////////////////////////////////////////////////////////
extern CLog* Log;
///////////////////////////////////////////////////////////////
#define Msg Log->Print
#define DbgMsg Log->Debug_Print
///////////////////////////////////////////////////////////////
