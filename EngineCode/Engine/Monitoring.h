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

		// ��������� ����������� � �����������
		ScopedTimer(const ScopedTimer&) = delete;
		ScopedTimer& operator=(const ScopedTimer&) = delete;

	  private:
		CMonitoring* m_Monitoring;
		ChartType m_Chart;
		CTimer m_Timer;
	};

	// �������� scoped �������
	[[nodiscard]] ScopedTimer CreateScopedTimer(ChartType chart)
	{
		return ScopedTimer(this, chart);
	}

	class CTaskMonitor
	{
	  private:
		std::unordered_map<std::string, size_t> task_counts;
		std::vector<std::string> task_names;
		std::mutex mtx;

	  public:
		// �������� ������ �� �����
		void addTask(const std::string& name)
		{
			std::lock_guard<std::mutex> lock(mtx);
			if (task_counts.find(name) == task_counts.end())
			{
				task_names.push_back(name);
				task_counts[name] = 1;
			}
			else
			{
				task_counts[name]++;
			}
		}

		// �������� ������ ��� ����������� �� �����
		void completeTask(const std::string& name)
		{
			std::lock_guard<std::mutex> lock(mtx);
			auto it = task_counts.find(name);
			if (it != task_counts.end())
			{
				if (--it->second == 0)
				{
					task_counts.erase(it);
					// ������� ��� �� �������
					for (auto vit = task_names.begin(); vit != task_names.end(); ++vit)
					{
						if (*vit == name)
						{
							task_names.erase(vit);
							break;
						}
					}
				}
			}
		}

		// �������� ����� ������ �������� �����
		std::vector<std::string> getActiveTasks()
		{
			std::lock_guard<std::mutex> lock(mtx);
			return task_names;
		}
	};

	CTaskMonitor TaskMonitor;

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
// ������ ��� �������� ������������� scoped �������
#define MONITOR_SCOPE(chart_type) auto ANONYMOUS_VARIABLE(monitoring_timer_) = Monitoring->CreateScopedTimer(chart_type)

// ��������������� ������ ��� ��������� ���������� ���
#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define ANONYMOUS_VARIABLE(prefix) CONCAT(prefix, __LINE__)

#define MONITORNG_CHART CMonitoring::ChartType
///////////////////////////////////////////////////////////////
