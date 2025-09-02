///////////////////////////////////////////////////////////////
//Created: 14.01.2025
//Author: NS_Deathman
//CApplication class realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
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
	int m_Frame;

	void HandleSDLEvents();
	void CalculateTimeStats();
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

	CApplication();
	~CApplication() = default;
};
///////////////////////////////////////////////////////////////
extern CApplication* App;
///////////////////////////////////////////////////////////////
