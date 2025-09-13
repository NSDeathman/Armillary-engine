///////////////////////////////////////////////////////////////
// Created: 29.08.2025
// Author: DeepSeek, NS_Deathman
// Profiling monitoring realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <array>
#include <string_view>
#include "Timer.h"
///////////////////////////////////////////////////////////////
class CMonitoring
{
  public:
	enum class ChartType : uint8_t
	{
		Render,
		Input,
		UI,
		Other,
		Count
	};

	static constexpr size_t CHART_HISTORY_SIZE = 100;
	static constexpr size_t CHART_COUNT = static_cast<size_t>(ChartType::Count);

	CMonitoring();
	~CMonitoring() = default;

	void OnFrame();
	void Draw();
	void CalcStatistic();

	void AddToChart(float time, ChartType chart);

	void SetNeedDrawMonitoring(bool flag)
	{
		m_NeedDraw = flag;
	}
	bool GetNeedDrawMonitoring() const
	{
		return m_NeedDraw;
	}

	class ScopedTimer
	{
	  public:
		ScopedTimer(CMonitoring* monitoring, ChartType chart) : m_Monitoring(monitoring), m_Chart(chart)
		{
			m_Timer.Start();
		}

		~ScopedTimer()
		{
			if (m_Monitoring)
			{
				float elapsedTime = m_Timer.GetElapsedTime();
				m_Monitoring->AddToChart(elapsedTime, m_Chart);
			}
		}

		// Запрещаем копирование и перемещение
		ScopedTimer(const ScopedTimer&) = delete;
		ScopedTimer& operator=(const ScopedTimer&) = delete;

	  private:
		CMonitoring* m_Monitoring;
		ChartType m_Chart;
		CTimer m_Timer;
	};

	// Создание scoped таймера
	[[nodiscard]] ScopedTimer CreateScopedTimer(ChartType chart)
	{
		return ScopedTimer(this, chart);
	}

  private:
	bool m_NeedDraw;
	float m_FPSAverage;
	size_t m_CurrentFrame;

	std::array<float, CHART_HISTORY_SIZE> m_FPSData;
	std::array<std::array<float, CHART_HISTORY_SIZE>, CHART_COUNT> m_ChartTimeData;
	std::array<float, CHART_COUNT> m_CurrentChartData;
	std::array<float, CHART_COUNT> m_StoredChartData;
	std::array<float, CHART_COUNT> m_ChartPercentages;

	static constexpr std::array<std::string_view, CHART_COUNT> CHART_LABELS = {"Render", "Input", "UI", "Other"};

	static float GetPercent(float a, float b)
	{
		return (a / b) * 100.0f;
	}
	static float ConvertToMS(float time)
	{
		return time * 1000.0f;
	}
};
///////////////////////////////////////////////////////////////
extern CMonitoring* Monitoring;
///////////////////////////////////////////////////////////////
// Макрос для удобного использования scoped таймера
#define MONITOR_SCOPE(chart_type) auto ANONYMOUS_VARIABLE(monitoring_timer_) = Monitoring->CreateScopedTimer(chart_type)

// Вспомогательный макрос для генерации уникальных имён
#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define ANONYMOUS_VARIABLE(prefix) CONCAT(prefix, __LINE__)

#define MONITORNG_CHART CMonitoring::ChartType
///////////////////////////////////////////////////////////////
