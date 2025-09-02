#pragma once
#include "stdafx.h"
//--------------------------------------------------------------------------------------
// Performs timer operations
// Use GetGlobalTimer() to get the global instance
//--------------------------------------------------------------------------------------
class CTimer
{
  public:
	CTimer();

	void Reset();			  // resets the timer
	void Start();			  // starts the timer
	void Stop();			  // stop (or pause) the timer
	void Advance();			  // advance the timer by 0.1 seconds
	double GetAbsoluteTime(); // get the absolute system time
	double GetTime();		  // get the current time
	float GetElapsedTime();	  // get the time that elapsed between Get*ElapsedTime() calls
	void GetTimeValues(double* pfTime, double* pfAbsoluteTime, float* pfElapsedTime); // get all time values at once
	bool IsStopped();																  // returns true if timer stopped

	// Limit the current thread to one processor (the current one). This ensures that timing code runs
	// on only one processor, and will not suffer any ill effects from power management.
	void LimitThreadAffinityToCurrentProc();

  protected:
	LARGE_INTEGER GetAdjustedCurrentTime();

	bool m_bUsingQPF;
	bool m_bTimerStopped;
	LONGLONG m_llQPFTicksPerSec;

	LONGLONG m_llStopTime;
	LONGLONG m_llLastElapsedTime;
	LONGLONG m_llBaseTime;
};

CTimer* WINAPI GetGlobalTimer();
