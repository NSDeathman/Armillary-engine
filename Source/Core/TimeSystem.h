///////////////////////////////////////////////////////////////
// Created: 21.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
//            _   _
//           (.)_(.)
//        _ (   _   ) _
//       / \/`-----'\/ \
//     __\ ( (     ) ) /__
//     )   /\ \._./ /\   (
//jgs   )_/ /|\   /|\ \_(
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "CoreAPI.h"
///////////////////////////////////////////////////////////////
namespace Core
{
	class CORE_API CTimeSystem : public Patterns::Singleton<CTimeSystem>
	{
		friend class Patterns::Singleton<CTimeSystem>;

	  public:
		// Основные методы
		void Initialize();
		void Update();

		// Свойства времени
		float GetDeltaTime() const;
		float GetTimeSinceStart() const;
		int GetFrameCount() const;

		// Управление FPS
		void SetTargetFPS(int fps);
		int GetTargetFPS() const;
		int GetCurrentFPS() const;

		// Ограничение FPS
		void EnableFPSCap(bool enable);
		bool IsFPSCapEnabled() const;

	  private:
		// Реализация (PIMPL)
		class Impl;
		Impl* m_Impl;
	};
} // namespace Core
///////////////////////////////////////////////////////////////
#define TIME_API_INIT() Core::CTimeSystem::GetInstance().Initialize()
#define TIME_API Core::CTimeSystem::GetInstance()
///////////////////////////////////////////////////////////////
