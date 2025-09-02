///////////////////////////////////////////////////////////////
// Created: 29.08.2025
// Author: DeepSeek, NS_Deathman
// Profiling monitoring realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Timer.h"
///////////////////////////////////////////////////////////////
#define MONITORING_CHART_RENDER 0
#define MONITORING_CHART_INPUT 1
#define MONITORING_CHART_UI 2
#define MONITORING_CHART_OTHER 3
#define MONITORING_CHARTS_COUNT 4
///////////////////////////////////////////////////////////////
class CMonitoring
{
private:
	bool m_NeedDraw;

	float fFPS_Average;
	int Frame;

public:
	void OnFrame();
	void Draw();
	void CalcStatistic();

	void AddToChart(float time, int chart);

	void SetNeedDrawMonitoring(bool flag)
	{
		m_NeedDraw = flag;
	}

	bool GetNeedDrawMonitoring()
	{
		return m_NeedDraw;
	}

	CMonitoring();
	~CMonitoring();
};
///////////////////////////////////////////////////////////////
extern CMonitoring* Monitoring;
///////////////////////////////////////////////////////////////
