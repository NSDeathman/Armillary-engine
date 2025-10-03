///////////////////////////////////////////////////////////////
// Created: 30.01.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#include "render_DX9.h"
#include "Log.h"
#include "render_backend_DX9_deprecated.h"
#include "OptickAPI.h"
#include "window_implementation.h"
///////////////////////////////////////////////////////////////
extern uint16_t g_ScreenWidth;
extern uint16_t g_ScreenHeight;
///////////////////////////////////////////////////////////////
void CRenderDX9::GetCapabilities()
{
	Log("\nGet render device capabilities");

	D3DCAPS9 Capabilities;
	m_pDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Capabilities);

	D3DADAPTER_IDENTIFIER9 adapterID;
	m_pDirect3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterID);
	Log("GPU [vendor:%X]-[device:%X]-[device name:%s]: %s", adapterID.VendorId, adapterID.DeviceId, adapterID.DeviceName, adapterID.Description);
	Log("Driver %s %d", adapterID.Driver, adapterID.DriverVersion);

	Log("Vertex shader version: %d.%d", D3DSHADER_VERSION_MAJOR(Capabilities.VertexShaderVersion), D3DSHADER_VERSION_MINOR(Capabilities.VertexShaderVersion));
	Log("Pixel shader version: %d.%d", D3DSHADER_VERSION_MAJOR(Capabilities.PixelShaderVersion), D3DSHADER_VERSION_MINOR(Capabilities.PixelShaderVersion));
	Log("Max vertex shader constants: %d", Capabilities.MaxVertexShaderConst);
	Log("Vertex shader instructions: %d", Capabilities.VertexShaderVersion ? 256 : 0);

	MaxSimultaneousTextures = Capabilities.MaxSimultaneousTextures;
	Log("Maximum supported simultaneous textures: %d", MaxSimultaneousTextures);

	MaxAnisotropy = Capabilities.MaxAnisotropy;
	Log("Maximum supported anisotropy: %d", MaxAnisotropy);

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
			Log("Maximum supported multi-samples: %d", samples);
			break;
		}
	}

	Log("\n");
}

void CRenderDX9::InitializeDirect3D()
{
	Log("Initializing Direct3D...");

	m_hWindow = DummyWindow->GetWindow();

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
	m_pDirect3DPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

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
		Log("Direct3D created successfully");
	else
		ERROR_MESSAGE("An error occurred while creating the Direct3D Device");

	// Set multisample parameters
	D3DMULTISAMPLE_TYPE multiSampleType = D3DMULTISAMPLE_NONE; // m_MaxMultiSamplingQuality;
	DWORD qualityLevels = 0;

	// Check if the selected multi-sample type is supported
	hresult = m_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, 
													  D3DDEVTYPE_HAL, 
													  D3DFMT_A8R8G8B8, 
													  FALSE, 
													  multiSampleType, 
													  &qualityLevels);

	if (SUCCEEDED(hresult))
		Log("Device multisample type checked successfully");
	else
		ERROR_MESSAGE("An error occurred while checking device multisample type");

	// Set the multi-sample settings in the present parameters
	m_pDirect3DPresentParams.MultiSampleType = multiSampleType;
	m_pDirect3DPresentParams.MultiSampleQuality = qualityLevels - 1;

	ShowWindow(m_hWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hWindow);

	ID3DXBuffer* vertexShaderBuffer = NULL;
	ID3DXBuffer* errorBuffer = NULL;

	string ObjectStageShaderPath = SHADERS + (string) "object_stage.hlsl";

	hresult = D3DXCompileShaderFromFile(ObjectStageShaderPath.c_str(), 
										nullptr, 
										nullptr, 
										"VSMain", 
										"vs_3_0", 
										D3DXSHADER_PACKMATRIX_ROWMAJOR | D3DXSHADER_PREFER_FLOW_CONTROL | D3DXSHADER_OPTIMIZATION_LEVEL3, 
										&vertexShaderBuffer, 
										&errorBuffer, 
										&m_pVertexShaderConstantTable);

	if (FAILED(hresult))
	{
		if (errorBuffer)
		{
			Log("Vertex shader error");
			Log("%s", (char*)errorBuffer->GetBufferPointer());
			MessageBoxA(nullptr, (char*)errorBuffer->GetBufferPointer(), "Shader Error", MB_OK);
			errorBuffer->Release();
		}
		return;
	}

	m_pDirect3dDevice->CreateVertexShader((DWORD*)vertexShaderBuffer->GetBufferPointer(), &m_vertexShader);
	vertexShaderBuffer->Release();

	ID3DXBuffer* PixelShaderBuffer = NULL;

	hresult = D3DXCompileShaderFromFile(ObjectStageShaderPath.c_str(), 
										nullptr, 
										nullptr, 
										"PSMain", 
										"ps_3_0", 
										D3DXSHADER_PACKMATRIX_ROWMAJOR | D3DXSHADER_PREFER_FLOW_CONTROL | D3DXSHADER_OPTIMIZATION_LEVEL3, 
										&PixelShaderBuffer,
										&errorBuffer, 
										&m_pPixelShaderConstantTable);

	if (FAILED(hresult))
	{
		if (errorBuffer)
		{
			Log("Pixel shader error");
			Log("%s", (char*)errorBuffer->GetBufferPointer());
			MessageBoxA(nullptr, (char*)errorBuffer->GetBufferPointer(), "Shader Error", MB_OK);
			errorBuffer->Release();
		}
		return;
	}

	m_pDirect3dDevice->CreatePixelShader((DWORD*)PixelShaderBuffer->GetBufferPointer(), &m_pixelShader);
	PixelShaderBuffer->Release();
}

void CRenderDX9::DestroyDirect3D()
{
	SAFE_RELEASE(m_pixelShader);
	SAFE_RELEASE(m_vertexShader);

	SAFE_RELEASE(m_pDirect3dDevice);

	SAFE_RELEASE(m_pDirect3D);
}

void CRenderDX9::ResetDirect3D()
{
	HRESULT result = m_pDirect3dDevice->Reset(&m_pDirect3DPresentParams);

	if (result == D3DERR_INVALIDCALL)
		ERROR_MESSAGE("Invalid call while device resetting");
}

void CRenderDX9::HandleDeviceLost()
{
	Log("Device was lost, resetting render...");

	HRESULT result = m_pDirect3dDevice->TestCooperativeLevel();

	if (result == D3DERR_DEVICELOST)
		Sleep(10);

	if (result == D3DERR_DEVICENOTRESET)
		Reset();

	m_bDeviceLost = false;
}
///////////////////////////////////////////////////////////////
