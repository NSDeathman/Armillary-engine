///////////////////////////////////////////////////////////////
// Created: 10.02.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#include "render_DX11.h"
#include "Log.h"
//#include "render_backend_DX11.h"
#include "OptickAPI.h"
#include "main_window.h"
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
void CRenderDX11::GetCapabilities()
{
	/*
	D3DCAPS9 Capabilities;

	m_pDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Capabilities);

	MaxSimultaneousTextures = Capabilities.MaxSimultaneousTextures;

	Msg("Maximum supported simultaneous textures: %d", MaxSimultaneousTextures);

	// Iterate through multi-sample types from highest to lowest
	for (int samples = D3DMULTISAMPLE_16_SAMPLES; samples >= D3DMULTISAMPLE_NONE; samples--)
	{
		DWORD qualityLevels = 0;

		HRESULT hr = m_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, FALSE,
															 (D3DMULTISAMPLE_TYPE)samples, &qualityLevels);

		// Check if this multi-sample type is supported and has quality levels
		if (SUCCEEDED(hr) && qualityLevels > 0)
		{
			m_MaxMultiSamplingQuality = (D3DMULTISAMPLE_TYPE)samples;
			Msg("Maximum supported multi-samples: %d", samples);
			break;
		}
	}
	*/
}

void CRenderDX11::InitializeDirect3D()
{
	Msg("Initializing Direct3D...");

	m_hWindow = MainWindow->GetWindow();

	ZeroMemory(&m_pDirect3dSwapChainDescription, sizeof(m_pDirect3dSwapChainDescription));
	m_pDirect3dSwapChainDescription.BufferCount = 1;
	m_pDirect3dSwapChainDescription.BufferDesc.Width = g_ScreenWidth;
	m_pDirect3dSwapChainDescription.BufferDesc.Height = g_ScreenHeight;
	m_pDirect3dSwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_pDirect3dSwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_pDirect3dSwapChainDescription.OutputWindow = m_hWindow;
	m_pDirect3dSwapChainDescription.SampleDesc.Count = 1;
	m_pDirect3dSwapChainDescription.Windowed = TRUE;

	D3D11CreateDeviceAndSwapChain(nullptr, 
								D3D_DRIVER_TYPE_HARDWARE, 
								nullptr, 
								0, 
								nullptr, 
								0, 
								D3D11_SDK_VERSION, 
								&m_pDirect3dSwapChainDescription, 
								&m_pDirect3dSwapChain, 
								&m_pDirect3dDevice, 
								nullptr, 
								&m_pDirect3dDeviceContext);

	ASSERT(m_pDirect3dDevice != NULL, "An error occurred while creating the Direct3D11");

	// Create a render target view
	ID3D11Texture2D* backBuffer;
	m_pDirect3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	m_pDirect3dDevice->CreateRenderTargetView(backBuffer, nullptr, &m_pDirect3dRenderTargetView);
	backBuffer->Release();

	ShowWindow(m_hWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hWindow);

	/*
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
	hresult = m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWindow,
										D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
										&m_pDirect3DPresentParams, &m_pDirect3dDevice);

	if (SUCCEEDED(hresult))
		Msg("Direct3D created successfully");
	else
		ERROR_MESSAGE("An error occurred while creating the Direct3D Device");

	// Set multisample parameters
	D3DMULTISAMPLE_TYPE multiSampleType = m_MaxMultiSamplingQuality;
	DWORD qualityLevels = 0;

	// Check if the selected multi-sample type is supported
	hresult = m_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, FALSE,
													  multiSampleType, &qualityLevels);

	if (SUCCEEDED(hresult))
		Msg("Device multisample type checked successfully");
	else
		ERROR_MESSAGE("An error occurred while checking device multisample type");

	// Set the multi-sample settings in the present parameters
	m_pDirect3DPresentParams.MultiSampleType = multiSampleType;
	m_pDirect3DPresentParams.MultiSampleQuality = qualityLevels - 1;

	ShowWindow(m_hWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hWindow);
	*/
}

void CRenderDX11::DestroyDirect3D()
{
	/*
	SAFE_RELEASE(m_pDirect3dDevice);

	SAFE_RELEASE(m_pDirect3D);
	*/
	SAFE_RELEASE(m_pDirect3dRenderTargetView);
	SAFE_RELEASE(m_pDirect3dSwapChain);
	SAFE_RELEASE(m_pDirect3dDeviceContext);
	SAFE_RELEASE(m_pDirect3dDevice);
}

void CRenderDX11::ResetDirect3D()
{
	/*
	HRESULT result = Device->Reset(&m_pDirect3DPresentParams);

	if (result == D3DERR_INVALIDCALL)
		ERROR_MESSAGE("Invalid call while device resetting");
	*/
}

void CRenderDX11::HandleDeviceLost()
{
	Msg("Device was lost, resetting render...");

	/*
	HRESULT result = Device->TestCooperativeLevel();

	if (result == D3DERR_DEVICELOST)
		Sleep(10);

	if (result == D3DERR_DEVICENOTRESET)
		Reset();

	m_bDeviceLost = false;
	*/
}
///////////////////////////////////////////////////////////////
