///////////////////////////////////////////////////////////////
// Created: 29.08.2025
// Author: DeepSeek, NS_Deathman
// Profiling monitoring realization
///////////////////////////////////////////////////////////////
#include "Monitoring.h"
#include "Application.h"
#include "imgui_api.h"
#include "log.h"
///////////////////////////////////////////////////////////////
extern CImguiAPI* Imgui;
///////////////////////////////////////////////////////////////
float FPS_Data[100] = {};
float RenderTimeData[100] = {};
float InputTimeData[100] = {};
float UITimeData[100] = {};
float OtherTimeData[100] = {};

LPCSTR ChartLabels[MONITORING_CHARTS_COUNT];
float ChartData[MONITORING_CHARTS_COUNT];
float ChartDataStored[MONITORING_CHARTS_COUNT];
float ChartDataPercents[MONITORING_CHARTS_COUNT];
///////////////////////////////////////////////////////////////
CMonitoring::CMonitoring()
{
#ifdef DEBUG_BUILD
	m_NeedDraw = true;
#else
	m_NeedDraw = false;
#endif
			
	fFPS_Average = 60.0f;
	Frame = 0;

	for (int i = 0; i < 99; i++)
	{
		FPS_Data[i] = 65.0f;
		RenderTimeData[i] = 100.0f;
		InputTimeData[i] = 100.0f;
		UITimeData[i] = 100.0f;
		OtherTimeData[i] = 100.0f;
	}

	ChartLabels[MONITORING_CHART_RENDER] = LPCSTR("Render");
	ChartLabels[MONITORING_CHART_INPUT] = LPCSTR("Input");
	ChartLabels[MONITORING_CHART_UI] = LPCSTR("UI");
	ChartLabels[MONITORING_CHART_OTHER] = LPCSTR("Other");

	for (int j = 0; j < MONITORING_CHARTS_COUNT; j++)
	{
		ChartData[j] = 1.0f;
		ChartDataStored[j] = 1.0f;
		ChartDataPercents[j] = ChartData[j];
	}
}

CMonitoring::~CMonitoring()
{
}

float GetPercent(float a, float b)
{
	return (a / b) * 100.0f;
}

float ConvertToMS(float time)
{
	return time * 1000.0f;
}

void CMonitoring::CalcStatistic()
{
	const float TimeDelta = App->GetTimeDelta();
	const float FrameTimeMS = ConvertToMS(App->GetFrameTime());
	const float FPS = App->GetFPS();

	FPS_Data[Frame] = FPS;

	for (int i = 0; i < 99; i++)
		fFPS_Average += FPS_Data[i];

	fFPS_Average /= 101.0f;

	float x = 0.0f;
	ChartData[MONITORING_CHART_OTHER] = 0.0f;

	for (int j = 0; j < MONITORING_CHARTS_COUNT; j++)
		x += ChartData[j];

	ChartData[MONITORING_CHART_OTHER] = App->GetFrameTime() - x;

	for (int k = 0; k < MONITORING_CHARTS_COUNT; k++)
	{
		ChartData[k] = ConvertToMS(ChartData[k]);
		ChartDataStored[k] = ChartData[k];
		ChartDataPercents[k] = GetPercent(ChartDataStored[k], FrameTimeMS);
	}

	RenderTimeData[Frame] = ChartDataPercents[MONITORING_CHART_RENDER];
	InputTimeData[Frame] = ChartDataPercents[MONITORING_CHART_INPUT];
	UITimeData[Frame] = ChartDataPercents[MONITORING_CHART_UI];
	OtherTimeData[Frame] = ChartDataPercents[MONITORING_CHART_OTHER];

	if (Frame < 99) [[likely]]
		Frame++;
	else
		Frame = 0;
}

void CMonitoring::AddToChart(float time, int chart)
{
	switch (chart)
	{
	case MONITORING_CHART_RENDER:
		ChartData[MONITORING_CHART_RENDER] = time;
		break;
	case MONITORING_CHART_INPUT:
		ChartData[MONITORING_CHART_INPUT] = time;
		break;
	case MONITORING_CHART_UI:
		ChartData[MONITORING_CHART_UI] = time;
		break;
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

	string FPSMessage = string("FPS: ") + std::to_string(App->GetFPS());
	string FPSAverageMessage = string("FPS Average: ") + std::to_string(fFPS_Average);

	ImGui::Text(FPSMessage.c_str());
	ImGui::Text(FPSAverageMessage.c_str());

	ImPlot::BeginPlot("Stats");
	ImPlot::PlotBars("FPS", FPS_Data, 100);
	ImPlot::EndPlot();

	string AllFrameTimeMessage = string("Frame time: ") + std::to_string(ConvertToMS(App->GetFrameTime())) + string(" ms ");
	string RenderFrameTimeMessage = string("Render: ") + std::to_string(ChartDataStored[MONITORING_CHART_RENDER]) + string(" ms (") + std::to_string(ChartDataPercents[MONITORING_CHART_RENDER]) + string(" percents)");
	string InputFrameTimeMessage = string("Input: ") + std::to_string(ChartDataStored[MONITORING_CHART_INPUT]) + string(" ms (") + std::to_string(ChartDataPercents[MONITORING_CHART_INPUT]) + string(" percents)");
	string UIFrameTimeMessage = string("UI: ") + std::to_string(ChartDataStored[MONITORING_CHART_UI]) + string(" ms (") + std::to_string(ChartDataPercents[MONITORING_CHART_UI]) + string(" percents)");
	string OtherFrameTimeMessage = string("Other: ") + std::to_string(ChartDataStored[MONITORING_CHART_OTHER]) + string(" ms (") + std::to_string(ChartDataPercents[MONITORING_CHART_OTHER]) + string(" percents)");

	ImGui::Text(AllFrameTimeMessage.c_str());
	ImGui::Text(RenderFrameTimeMessage.c_str());
	ImGui::Text(InputFrameTimeMessage.c_str());
	ImGui::Text(UIFrameTimeMessage.c_str());
	ImGui::Text(OtherFrameTimeMessage.c_str());

	static ImPlotPieChartFlags flags = ImPlotFlags_NoTitle | ImPlotFlags_NoMouseText | ImPlotPieChartFlags_Normalize | ImPlotPieChartFlags_IgnoreHidden;

	//ImPlot::PushColormap(ImPlotColormap_Dark);
	//ImPlot::BeginPlot("Frametime chart:", ImVec2(-1, 0), flags);
	//ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
	//ImPlot::SetupAxesLimits(0, 1, 0, 1);
	//ImPlot::PlotPieChart(ChartLabels, ChartDataPercents, MONITORING_CHARTS_COUNT, 0.5, 0.5, 0.4, "%.0f", 180, flags);
	//ImPlot::EndPlot();
	//ImPlot::PopColormap();

	ImPlot::BeginPlot("Chart");
	ImPlot::PlotBars("Render", RenderTimeData, 100);
	ImPlot::PlotBars("Input", InputTimeData, 100);
	ImPlot::PlotBars("UI", UITimeData, 100);
	ImPlot::PlotBars("Other", OtherTimeData, 100);
	ImPlot::EndPlot();

	ImGui::PopFont();

	ImGui::End();
}
///////////////////////////////////////////////////////////////
