///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
///////////////////////////////////////////////////////////////
#include <iostream>
#include <ppl.h>
#include <cstddef>
#include <Windows.h>
#include <mmsystem.h>
#include <vector>
#include <array>
#include <iomanip>
#include <ctime>
#include <string>
#include <random>
#include <csignal>
#include <cstdlib>
#include <sstream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

#pragma warning(disable : 4996) // disable deprecated warning
#include <strsafe.h>
#pragma warning(default : 4996)
///////////////////////////////////////////////////////////////
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dmoguids.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "msdmo.lib")
#pragma comment(lib, "Strmiids.lib")
///////////////////////////////////////////////////////////////
#include <SDL/SDL.h>
///////////////////////////////////////////////////////////////
#pragma comment(lib, "SDL2.lib")
///////////////////////////////////////////////////////////////
