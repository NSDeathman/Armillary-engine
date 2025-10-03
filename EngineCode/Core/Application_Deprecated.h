///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//CApplication class realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Timer.h"
///////////////////////////////////////////////////////////////
class CApplication
{
private:
	CTimer m_Timer;
	float m_LastTime;
	float m_CurrentTime;
	float m_TimeDelta;
	float m_FrameTime;
	float m_FPS;
	float m_FPSLimit;
	int m_Frame;

	void HandleSDLEvents();
	void CalculateTimeStats();
	void ProcessFrameLimiter();
	void OnFrame();
	void EventLoop();

public:
	void Start();
	void Destroy();
	void Process();

	float GetTimeDelta()
	{
		return m_TimeDelta;
	}

	int GetFrames()
	{
		return m_Frame;
	}

	float GetTime()
	{
		return (float)m_Timer.GetTime();
	}

	float GetFPS()
	{
		return m_FPS;
	}

	float GetFrameTime()
	{
		return m_FrameTime;
	}

	void SetFPSLimit(float FPS)
	{
		m_FPSLimit = FPS;
	}

	float GetFPSLimit()
	{
		return m_FPSLimit;
	}

	CApplication();
	~CApplication() = default;
};
///////////////////////////////////////////////////////////////
extern CApplication* Application;
///////////////////////////////////////////////////////////////
