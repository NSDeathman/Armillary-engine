///////////////////////////////////////////////////////////////
// Created: 30.01.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#include "render.h"
#include "Log.h"
#include "render_backend.h"
#include "OptickAPI.h"
#include "main_window.h"
///////////////////////////////////////////////////////////////
extern UINT g_ScreenWidth;
extern UINT g_ScreenHeight;
///////////////////////////////////////////////////////////////
void CRender::GetCapabilities()
{
	D3DCAPS9 Capabilities;

	m_pDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Capabilities);

	MaxSimultaneousTextures = Capabilities.MaxSimultaneousTextures;

	Msg("Maximum supported simultaneous textures: %d", MaxSimultaneousTextures);

	// Iterate through multi-sample types from highest to lowest
	for (int samples = D3DMULTISAMPLE_16_SAMPLES; samples >= D3DMULTISAMPLE_NONE; samples--)
	{
		DWORD qualityLevels = 0;

		HRESULT hr = m_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, 
															 D3DDEVTYPE_HAL,
															 D3DFMT_A8R8G8B8,
															 FALSE,
															 (D3DMULTISAMPLE_TYPE)samples, 
															 &qualityLevels);

		// Check if this multi-sample type is supported and has quality levels
		if (SUCCEEDED(hr) && qualityLevels > 0)
		{
			m_MaxMultiSamplingQuality = (D3DMULTISAMPLE_TYPE)samples;
			Msg("Maximum supported multi-samples: %d", samples);
			break;
		}
	}
}

void CRender::InitializeDirect3D()
{
	Msg("Initializing Direct3D...");

	m_hWindow = MainWindow->GetWindow();

	// Create the D3D object.
	m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

	ASSERT(m_pDirect3D != NULL, "An error occurred while creating the Direct3D9");

	// Set up the structure used to create the D3DDevice. Since we are now
	// using more complex geometry, we will create a device with a zbuffer.
	ZeroMemory(&m_pDirect3DPresentParams, sizeof(m_pDirect3DPresentParams));
	m_pDirect3DPresentParams.Windowed = TRUE;
	m_pDirect3DPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_pDirect3DPresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
	m_pDirect3DPresentParams.EnableAutoDepthStencil = TRUE;
	m_pDirect3DPresentParams.AutoDepthStencilFormat = D3DFMT_D24X8;

	GetCapabilities();

	HRESULT hresult = E_FAIL;

	// Create the D3DDevice
	hresult = m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, 
										D3DDEVTYPE_HAL, 
										m_hWindow,
										D3DCREATE_HARDWARE_VERTEXPROCESSING | 
										D3DCREATE_MULTITHREADED,
										&m_pDirect3DPresentParams, 
										&m_pDirect3dDevice);

	if (SUCCEEDED(hresult))
		Msg("Direct3D created successfully");
	else
		ERROR_MESSAGE("An error occurred while creating the Direct3D Device");

	// Set multisample parameters
	D3DMULTISAMPLE_TYPE multiSampleType = m_MaxMultiSamplingQuality;
	DWORD qualityLevels = 0;

	// Check if the selected multi-sample type is supported
	hresult = m_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, 
													  D3DDEVTYPE_HAL, 
													  D3DFMT_A8R8G8B8, 
													  FALSE, 
													  multiSampleType, 
													  &qualityLevels);

	if (SUCCEEDED(hresult))
		Msg("Device multisample type checked successfully");
	else
		ERROR_MESSAGE("An error occurred while checking device multisample type");

	// Set the multi-sample settings in the present parameters
	m_pDirect3DPresentParams.MultiSampleType = multiSampleType;
	m_pDirect3DPresentParams.MultiSampleQuality = qualityLevels - 1;

	ShowWindow(m_hWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hWindow);
}

void CRender::DestroyDirect3D()
{
	SAFE_RELEASE(m_pDirect3dDevice);

	SAFE_RELEASE(m_pDirect3D);
}

void CRender::ResetDirect3D()
{
	HRESULT result = Device->Reset(&m_pDirect3DPresentParams);

	if (result == D3DERR_INVALIDCALL)
		ERROR_MESSAGE("Invalid call while device resetting");
}

void CRender::HandleDeviceLost()
{
	Msg("Device was lost, resetting render...");

	HRESULT result = Device->TestCooperativeLevel();

	if (result == D3DERR_DEVICELOST)
		Sleep(10);

	if (result == D3DERR_DEVICENOTRESET)
		Reset();

	m_bDeviceLost = false;
}
///////////////////////////////////////////////////////////////
