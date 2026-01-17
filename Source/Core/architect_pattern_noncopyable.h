///////////////////////////////////////////////////////////////
// Created: 23.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "CoreMacros.h"
///////////////////////////////////////////////////////////////
CORE_BEGIN
namespace Patterns
{
	class Noncopyable
	{
		protected:
		Noncopyable() = default;
		~Noncopyable() = default;

		// Запрещаем копирование
		Noncopyable(const Noncopyable&) = delete;
		Noncopyable& operator=(const Noncopyable&) = delete;

		// Разрешаем перемещение
		Noncopyable(Noncopyable&&) = default;
		Noncopyable& operator=(Noncopyable&&) = default;
	};
} // namespace Patterns
CORE_END
///////////////////////////////////////////////////////////////
