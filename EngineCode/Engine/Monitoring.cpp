///////////////////////////////////////////////////////////////
// Created: 29.08.2025
// Author: DeepSeek, NS_Deathman
// Profiling monitoring realization
///////////////////////////////////////////////////////////////
#include "Monitoring.h"
#include "Application.h"
#include "imgui_api.h"
#include "log.h"
#include <algorithm>
#include <numeric>
///////////////////////////////////////////////////////////////
extern CImguiAPI* Imgui;
///////////////////////////////////////////////////////////////
CMonitoring::CMonitoring() : m_NeedDraw(false), m_FPSAverage(60.0f), m_CurrentFrame(0)
{
#ifdef DEBUG_BUILD
	m_NeedDraw = true;
#endif

	// Инициализация данных
	std::fill(m_FPSData.begin(), m_FPSData.end(), 65.0f);

	for (auto& chartData : m_ChartTimeData)
	{
		std::fill(chartData.begin(), chartData.end(), 100.0f);
	}

	std::fill(m_CurrentChartData.begin(), m_CurrentChartData.end(), 1.0f);
	std::fill(m_StoredChartData.begin(), m_StoredChartData.end(), 1.0f);
	std::fill(m_ChartPercentages.begin(), m_ChartPercentages.end(), 1.0f);
}

void CMonitoring::CalcStatistic()
{
	const float frameTime = App->GetFrameTime();
	const float frameTimeMS = ConvertToMS(frameTime);
	const float fps = App->GetFPS();

	// Обновление данных FPS
	m_FPSData[m_CurrentFrame] = fps;
	m_FPSAverage = std::accumulate(m_FPSData.begin(), m_FPSData.end(), 0.0f) / CHART_HISTORY_SIZE;

	// Вычисление времени для "Other"
	float totalTrackedTime = 0.0f;
	for (size_t i = 0; i < CHART_COUNT - 1; ++i) // исключаем Other
	{
		totalTrackedTime += m_CurrentChartData[i];
	}
	m_CurrentChartData[static_cast<size_t>(ChartType::Other)] = max(0.0f, frameTime - totalTrackedTime);

	// Конвертация в миллисекунды и проценты
	for (size_t i = 0; i < CHART_COUNT; ++i)
	{
		m_StoredChartData[i] = ConvertToMS(m_CurrentChartData[i]);
		m_ChartPercentages[i] = GetPercent(m_StoredChartData[i], frameTimeMS);
		m_ChartTimeData[i][m_CurrentFrame] = m_ChartPercentages[i];
	}

	// Обновление индекса кадра
	m_CurrentFrame = (m_CurrentFrame + 1) % CHART_HISTORY_SIZE;
}

void CMonitoring::AddToChart(float time, ChartType chart)
{
	const size_t index = static_cast<size_t>(chart);
	if (index < CHART_COUNT - 1) // Не позволяем напрямую устанавливать Other
	{
		m_CurrentChartData[index] = time;
	}
}

void CMonitoring::OnFrame()
{
	CalcStatistic();
}

void CMonitoring::Draw()
{
	if (!m_NeedDraw)
		return;

	ImGui::PushFont(Imgui->font_letterica_big);
	ImGui::Begin("Monitoring window", &m_NeedDraw);
	ImGui::PopFont();

	ImGui::PushFont(Imgui->font_letterica_medium);

	// Отображение FPS
	ImGui::Text("FPS: %.1f", App->GetFPS());
	ImGui::Text("FPS Average: %.1f", m_FPSAverage);

	// График FPS
	if (ImPlot::BeginPlot("FPS Stats", "Frame", "FPS", ImVec2(-1, 200)))
	{
		ImPlot::PlotLine("FPS", m_FPSData.data(), static_cast<int>(m_FPSData.size()));
		ImPlot::EndPlot();
	}

	// Время кадра
	const float frameTimeMS = ConvertToMS(App->GetFrameTime());
	ImGui::Text("Frame time: %.2f ms", frameTimeMS);

	// Детализация по категориям
	for (size_t i = 0; i < CHART_COUNT; ++i)
	{
		ImGui::Text("%s: %.2f ms (%.1f%%)", CHART_LABELS[i].data(), m_StoredChartData[i], m_ChartPercentages[i]);
	}

	// График распределения времени
	if (ImPlot::BeginPlot("Time Distribution", "Frame", "Percentage", ImVec2(-1, 200)))
	{
		for (size_t i = 0; i < CHART_COUNT; ++i)
		{
			ImPlot::PlotLine(CHART_LABELS[i].data(), m_ChartTimeData[i].data(),
							 static_cast<int>(m_ChartTimeData[i].size()));
		}
		ImPlot::EndPlot();
	}

	ImGui::PopFont();
	ImGui::End();
}
///////////////////////////////////////////////////////////////
