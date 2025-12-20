///////////////////////////////////////////////////////////////
// Created: 21.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TimeSystem.h"
///////////////////////////////////////////////////////////////
namespace Core
{
	class CTimeSystem::Impl
	{
	  public:
		std::chrono::high_resolution_clock::time_point startTime;
		std::chrono::high_resolution_clock::time_point lastFrameTime;
		float deltaTime = 0.0f;
		int frameCount = 0;
		int targetFPS = 60;
		int currentFPS = 0;
		bool fpsCapEnabled = true;

		// Для расчета FPS
		float fpsAccumulator = 0.0f;
		int fpsFrames = 0;
		std::chrono::high_resolution_clock::time_point lastFPSUpdate;
	};

	void CTimeSystem::Initialize()
	{
		m_Impl = new Impl();
		m_Impl->startTime = std::chrono::high_resolution_clock::now();
		m_Impl->lastFrameTime = m_Impl->startTime;
		m_Impl->lastFPSUpdate = m_Impl->startTime;
	}

	void CTimeSystem::Update()
	{
		auto currentTime = std::chrono::high_resolution_clock::now();

		// Расчет delta time
		auto delta = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_Impl->lastFrameTime);
		m_Impl->deltaTime = delta.count() / 1000000.0f;

		m_Impl->lastFrameTime = currentTime;
		m_Impl->frameCount++;

		// Расчет FPS
		m_Impl->fpsFrames++;
		auto fpsDelta = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_Impl->lastFPSUpdate);

		if (fpsDelta.count() >= 1.0f)
		{
			m_Impl->currentFPS = m_Impl->fpsFrames;
			m_Impl->fpsFrames = 0;
			m_Impl->lastFPSUpdate = currentTime;
		}

		// Ограничение FPS
		if (m_Impl->fpsCapEnabled && m_Impl->targetFPS > 0)
		{
			float targetFrameTime = 1.0f / m_Impl->targetFPS;
			if (m_Impl->deltaTime < targetFrameTime)
			{
				auto sleepTime = std::chrono::microseconds(static_cast<int>((targetFrameTime - m_Impl->deltaTime) * 1000000));
				std::this_thread::sleep_for(sleepTime);
			}
		}
	}

	float CTimeSystem::GetDeltaTime() const
	{
		return m_Impl->deltaTime;
	}
	float CTimeSystem::GetTimeSinceStart() const
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_Impl->startTime);
		return duration.count() / 1000000.0f;
	}
	int CTimeSystem::GetFrameCount() const
	{
		return m_Impl->frameCount;
	}

	void CTimeSystem::SetTargetFPS(int fps)
	{
		m_Impl->targetFPS = fps;
	}
	int CTimeSystem::GetTargetFPS() const
	{
		return m_Impl->targetFPS;
	}
	int CTimeSystem::GetCurrentFPS() const
	{
		return m_Impl->currentFPS;
	}

	void CTimeSystem::EnableFPSCap(bool enable)
	{
		m_Impl->fpsCapEnabled = enable;
	}
	bool CTimeSystem::IsFPSCapEnabled() const
	{
		return m_Impl->fpsCapEnabled;
	}
} // namespace Core
///////////////////////////////////////////////////////////////
