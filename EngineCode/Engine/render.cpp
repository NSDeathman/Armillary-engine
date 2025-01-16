///////////////////////////////////////////////////////////////
// Created: 15.01.2025
// Author: NS_Deathman
// Renderer realization
///////////////////////////////////////////////////////////////
#include "render.h"
#include "Log.h"
#include "filesystem.h"
#include "resource.h"
#include "render_backend.h"
///////////////////////////////////////////////////////////////
extern LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
///////////////////////////////////////////////////////////////
CRender::CRender()
{
	m_hWindow = NULL;
	m_pDirect3D = NULL;
	m_pDirect3dDevice = NULL;
	ZeroMemory(&m_pDirect3DPresentParams, sizeof(m_pDirect3DPresentParams));

	m_pMesh = NULL;
	m_pMeshMaterials.resize(NULL);
	m_pMeshTextures.resize(NULL);
	m_dwNumMaterials = 0L;

	m_bDeviceLost = false;
}

void CRender::Destroy()
{
	Log->Print("Destroying render...");

	if (!m_pMeshTextures.empty())
	{
		concurrency::parallel_for(DWORD(0), m_dwNumMaterials, [this](u32 iterator) 
		{
			if (m_pMeshTextures[iterator])
				m_pMeshTextures[iterator]->Release();
		});

		m_pMeshTextures.clear();
	}

	if (!m_pMeshMaterials.empty())
		m_pMeshMaterials.clear();

	RELEASE(m_pDirect3dDevice);

	RELEASE(m_pDirect3D);
}

void CRender::CreateMainWindow()
{
	Log->Print("Creating window...");

	WNDCLASSEX wc;
	// The window class. This has to be filled BEFORE the window can be WNDCLASSEX wc;
	//Flags[Redraw on width / height change from resize / movement] 
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	// Pointer to the window processing function for handling messages from this window
	wc.lpfnWndProc = MsgProc;
	// Number of extra bytes to allocate following the window-class structure
	wc.cbClsExtra = 0;
	// Number of extra bytes to allocate following the window instance
	wc.cbWndExtra = 0;

	// Handle to the instance that contains the window procedure
	wc.hInstance = GetModuleHandle(NULL);
	// Handle to the class icon. Must be a handle to an Icon resource
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	// Handle to the small icon for the class
	wc.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	// Handle to the class cursor. If null, an application must explicitly set the cursor shape whenever the mouse moves
	// into the application window
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// Handle to the class background brush for the window's background colour. When NULL an application must paint its
	// own background colour
	wc.hbrBackground = NULL;
	// Pointer to a null-terminated string for the menu
	wc.lpszMenuName = NULL;
	// Pointer to null-terminated string of our class name
	wc.lpszClassName = "Atlas";
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	// Create the application's window
	m_hWindow = CreateWindow("ATLAS", 
							 "Atlas engine", 
							 WS_OVERLAPPEDWINDOW, 
							 100,	
							 100, 
							 640, 
							 480, 
							 NULL, 
							 NULL, 
							 wc.hInstance, 
							 NULL);

	ASSERT(!GetLastError(), "An error occurred while creating the window");
}

void CRender::InitializeDirect3D()
{
	Log->Print("Initializing Direct3D...");

	// Create the D3D object.
	m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);

	ASSERT(!(m_pDirect3D == NULL), "An error occurred while creating the Direct3D");

	// Set up the structure used to create the D3DDevice. Since we are now
	// using more complex geometry, we will create a device with a zbuffer.
	ZeroMemory(&m_pDirect3DPresentParams, sizeof(m_pDirect3DPresentParams));
	m_pDirect3DPresentParams.Windowed = TRUE;
	m_pDirect3DPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_pDirect3DPresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
	m_pDirect3DPresentParams.EnableAutoDepthStencil = TRUE;
	m_pDirect3DPresentParams.AutoDepthStencilFormat = D3DFMT_D16;

	// Create the D3DDevice
	HRESULT hresult = m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, 
												D3DDEVTYPE_HAL, 
												m_hWindow, 
												D3DCREATE_HARDWARE_VERTEXPROCESSING,
												&m_pDirect3DPresentParams, 
												&m_pDirect3dDevice);

	ASSERT(SUCCEEDED(hresult), "An error occurred while creating the Direct3D");

	if (SUCCEEDED(hresult))
		Log->Print("Direct3D created successfully");

	ShowWindow(m_hWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hWindow);

	GetCapabilities();
}

void CRender::GetCapabilities()
{
	D3DCAPS9 Capabilities;
	m_pDirect3dDevice->GetDeviceCaps(&Capabilities);

	MaxSimultaneousTextures = Capabilities.MaxSimultaneousTextures;
}

void CRender::CreateMatrices()
{
	// Set up world matrix
	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
	m_pDirect3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// Set up our view matrix. A view matrix can be defined given an eye point,
	// a point to lookat, and a direction for which way is up. Here, we set the
	// eye five units back along the z-axis and up three units, look at the
	// origin, and define "up" to be in the y-direction.
	D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	m_pDirect3dDevice->SetTransform(D3DTS_VIEW, &matView);

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	m_pDirect3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

void CRender::Initialize()
{
	Log->Print("Initializing render...");
	CreateMainWindow();
	InitializeDirect3D();
	LoadScene();
}

void CRender::Reset()
{
	HRESULT result = m_pDirect3dDevice->Reset(&m_pDirect3DPresentParams);

	if (result == D3DERR_INVALIDCALL)
		ERROR_MESSAGE("Invalid call while device resetting");
}

void CRender::HandleDeviceLost()
{
	Log->Print("Device was lost, resetting render...");

	HRESULT result = m_pDirect3dDevice->TestCooperativeLevel();

	if (result == D3DERR_DEVICELOST)
		Sleep(10);

	if (result == D3DERR_DEVICENOTRESET)
		Reset();

	m_bDeviceLost = false;
}

void CRender::RenderFrame()
{
	OPTICK_EVENT("CRender::RenderFrame")

	if (m_bDeviceLost)
		HandleDeviceLost();

	// Clear the backbuffer and the zbuffer
	m_pDirect3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	// Setup the world, view, and projection matrices
	CreateMatrices();

	// Begin the scene
	HRESULT hresult = m_pDirect3dDevice->BeginScene();

	if (FAILED(hresult))
		Log->Print("Failed to begin scene render");

	// Turn on the zbuffer
	m_pDirect3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	RenderBackend->set_CullMode(CBackend::CULL_CCW);

	// Depth prepass
	{
		RenderBackend->set_ZWriteEnable(TRUE);

		m_pDirect3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

		RenderScene();

		RenderBackend->set_ZWriteEnable(FALSE);

		m_pDirect3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
	}

	m_pDirect3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	m_pDirect3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_PHONG);

	RenderScene();

	// End the scene
	m_pDirect3dDevice->EndScene();

	// Present the backbuffer contents to the display
	HRESULT present_result = m_pDirect3dDevice->Present(NULL, NULL, NULL, NULL);

	if (present_result == D3DERR_DEVICELOST)
		m_bDeviceLost = true;
}

void CRender::LoadScene()
{
	Log->Print("Loading scene...");

	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	HRESULT hresult = E_FAIL;

	hresult = D3DXLoadMeshFromX("..\\GameResources\\Tiger.x", 
								D3DXMESH_SYSTEMMEM, 
								m_pDirect3dDevice, 
								NULL,
								&pD3DXMtrlBuffer, 
								NULL, 
								&m_dwNumMaterials, 
								&m_pMesh);

	// If model is not in current folder
	ASSERT(SUCCEEDED(hresult), "Could not find tiger.x")

	// We need to extract the material properties and texture names from the pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMeshMaterials.resize(m_dwNumMaterials);

	ASSERT(!m_pMeshMaterials.empty(), "Can`t load mesh materials")

	m_pMeshTextures.resize(m_dwNumMaterials);

	ASSERT(!m_pMeshTextures.empty(), "Can`t load mesh textures")

	concurrency::parallel_for(DWORD(0), m_dwNumMaterials, [&](u32 iterator) 
	{
		// Copy the material
		m_pMeshMaterials[iterator] = d3dxMaterials[iterator].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		m_pMeshMaterials[iterator].Ambient = m_pMeshMaterials[iterator].Diffuse;

		m_pMeshTextures[iterator] = NULL;
		if (d3dxMaterials[iterator].pTextureFilename != NULL && lstrlenA(d3dxMaterials[iterator].pTextureFilename) > 0)
		{
			HRESULT hresult = E_FAIL;

			// Create texture path
			CHAR strTexture[MAX_PATH];
			strcpy_s(strTexture, MAX_PATH, GAME_RESOURCES);
			strcat_s(strTexture, MAX_PATH, d3dxMaterials[iterator].pTextureFilename);

			// Create the texture
			hresult = D3DXCreateTextureFromFileA(m_pDirect3dDevice, strTexture, &m_pMeshTextures[iterator]);

			ASSERT(SUCCEEDED(hresult), "Could not find texture map")
		}
	});

	// Done with the material buffer
	pD3DXMtrlBuffer->Release();

	Log->Print("Scene loaded successfully");
}

void CRender::RenderScene()
{
	// Meshes are divided into subsets, one for each material. Render them in a loop
	concurrency::parallel_for(DWORD(0), m_dwNumMaterials, [this](u32 iterator)
	{
		// Set the material and texture for this subset
		m_pDirect3dDevice->SetMaterial(&m_pMeshMaterials[iterator]);
		m_pDirect3dDevice->SetTexture(0, m_pMeshTextures[iterator]);

		// Draw the mesh subset
		m_pMesh->DrawSubset(iterator);
	});
}
///////////////////////////////////////////////////////////////
