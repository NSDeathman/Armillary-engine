#pragma once
#include "framework.h"

void LogDebug(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    OutputDebugStringA(buffer); // <--- ѕишет в окно Output
    OutputDebugStringA("\n");
}