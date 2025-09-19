///////////////////////////////////////////////////////////////////////////////////
// Created: 05.01.2025
// Author: NS_Deathman
// Optick profiling implementation
///////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////////////////////////
#include "OptickAPI.h"
#include "Log.h"
#include "Application.h"
///////////////////////////////////////////////////////////////////////////////////
void COptickAPI::Initialize()
{
	Msg("\n");
	Msg("Initializing OptickCapture");
	m_frames_to_capture = -1;
	m_start_capture_frame = -1;
	m_end_capture_frame = -1;
	m_need_capture = false;
	m_switched_to_capturing = false;
};

void COptickAPI::Destroy()
{
	if (m_need_capture)
		StopCapturing();
};

/*********************************************************************************
Base methods: Start, Stop, Save
*********************************************************************************/
void COptickAPI::StartCapturing()
{
	OPTICK_START_CAPTURE(Optick::Mode::Type(Optick::Mode::INSTRUMENTATION | 
											Optick::Mode::TAGS | 
											Optick::Mode::AUTOSAMPLING | 
											Optick::Mode::SWITCH_CONTEXT |
											Optick::Mode::IO | 
											Optick::Mode::SYS_CALLS | 
											Optick::Mode::OTHER_PROCESSES));

	Msg("- Optick capturing started");
}

/* Saving in try-statement for avoiding game crash */
#pragma warning(disable : 4100)
void COptickAPI::TryToSaveCapture(str_c save_path)
{
	try
	{
		OPTICK_SAVE_CAPTURE(save_path);
	}
	catch (...)
	{
		Msg("! An error occurred while saving optick capture");
	}
};
#pragma warning(default : 4100)

void COptickAPI::SaveCapture(str_c save_path)
{
	TryToSaveCapture(save_path);
	Msg("- Optick capture saved with name: %s", save_path);
};

/*********************************************************************************
Capturing a specified number of frames.
*********************************************************************************/
void COptickAPI::OnFrame()
{
	if (m_need_capture)
	{
		if (App->GetFrames() == m_end_capture_frame) [[unlikely]]
		{
			StopCapturing();
			SaveCapturedFrames();
		}
	}
};

void COptickAPI::StartCapturing(int frames_to_capture)
{
	m_need_capture = true;
	m_frames_to_capture = frames_to_capture;
	m_start_capture_frame = App->GetFrames();
	m_end_capture_frame = m_start_capture_frame + m_frames_to_capture;

	StartCapturing();
};

void COptickAPI::StopCapturing()
{
	OPTICK_STOP_CAPTURE();
	m_need_capture = false;

	Msg("- Optick capturing stoped");
};

void COptickAPI::SaveCapturedFrames()
{
	Msg("- Saving captured frames");

	char capture_path[100];
	LPCSTR frames = m_frames_to_capture == 1 ? "frame" : "frames";
	sprintf(capture_path, "optick_capture_%d_%s.opt", m_frames_to_capture, frames);

	SaveCapture(capture_path);
};

/*********************************************************************************
Capturing in switcher mode:
	first call switching to capture, second switching to save
*********************************************************************************/
void COptickAPI::SwitchProfiler()
{
	if (m_need_capture)
	{
		Msg("! Capturing already started, please wait until end of capturing and try again");
		return;
	}

	if (!m_switched_to_capturing)
	{
		Msg("- OptickAPI switched to capturing mode, execute command again to switch to saving mode");
		SwitchToCapturing();
		m_switched_to_capturing = true;
	}
	else
	{
		Msg("- OptickAPI switched to saving mode");
		SwitchToSaving();
		m_switched_to_capturing = false;
	}
};

void COptickAPI::SwitchToCapturing()
{
	m_start_capture_frame = App->GetFrames();

	StartCapturing();
};

void COptickAPI::SwitchToSaving()
{
	StopCapturing();

	m_frames_to_capture = App->GetFrames() - m_start_capture_frame;

	Msg("- Saving %d frames", m_frames_to_capture);

	char capture_path[100];
	LPCSTR frames = m_frames_to_capture == 1 ? "frame" : "frames";
	sprintf(capture_path, "optick_capture_%d_%s.opt", m_frames_to_capture, frames);

	SaveCapture(capture_path);
};
///////////////////////////////////////////////////////////////////////////////////
COptickAPI* OptickAPI = nullptr;
///////////////////////////////////////////////////////////////////////////////////
