#pragma once

#ifdef RENDERUCTOR_EXPORTS
#define RENDER_API __declspec(dllexport)
#else
#define RENDER_API __declspec(dllimport)
#endif

// Отключаем предупреждение об экспорте STL классов (std::string, std::map)
// Это безопасно, если DLL и EXE собраны одной версией компилятора (VS)
#pragma warning(disable: 4251)
