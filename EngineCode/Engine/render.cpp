///////////////////////////////////////////////////////////////
//Created: 15.01.2025
//Author: NS_Deathman
//Renderer realization
///////////////////////////////////////////////////////////////
#include "render.h"
#include "Log.h"
///////////////////////////////////////////////////////////////
extern LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
///////////////////////////////////////////////////////////////
CRender::CRender()
{
	m_hWindow = NULL;
    m_pDirect3D = NULL;
    m_pDirect3dDevice = NULL;

	m_pMesh = NULL;
	m_pMeshMaterials = NULL;
	m_pMeshTextures = NULL;
	m_dwNumMaterials = 0L;
}

CRender::~CRender()
{
    Log->Print("Destroying render...");

	if (m_pMeshMaterials != NULL)
		delete[] m_pMeshMaterials;

	if (m_pMeshTextures)
	{
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			if (m_pMeshTextures[i])
				m_pMeshTextures[i]->Release();
		}
		delete[] m_pMeshTextures;
	}

    if (m_pDirect3dDevice != NULL)
        m_pDirect3dDevice->Release();

    if (m_pDirect3D != NULL)
        m_pDirect3D->Release();
}

void CRender::CreateMainWindow()
{
    Log->Print("Creating window...");

    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Atlas", NULL
    };
    RegisterClassEx(&wc);

    // Create the application's window
    m_hWindow = CreateWindow(L"ATLAS", L"Main window", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);
}

void CRender::InitializeDirect3D()
{
    Log->Print("Initializing Direct3D...");

    // Create the D3D object.
    if (NULL == (m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS Direct3DPresentParams;
    ZeroMemory(&Direct3DPresentParams, sizeof(Direct3DPresentParams));
    Direct3DPresentParams.Windowed = TRUE;
    Direct3DPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    Direct3DPresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
    Direct3DPresentParams.EnableAutoDepthStencil = TRUE;
    Direct3DPresentParams.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    HRESULT hresult = m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &Direct3DPresentParams, &m_pDirect3dDevice);

    if (FAILED(hresult))
    {
        Log->Print("Fail while creating Direct3D");
        return;
    }
    else
    {
        Log->Print("Direct3D created successfuly");
    }

    // Turn on the zbuffer
    m_pDirect3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    // Turn on ambient lighting 
    m_pDirect3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

    return;
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

void CRender::RenderFrame()
{
	Log->Print("Frame");

	// Clear the backbuffer and the zbuffer
	m_pDirect3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	// Begin the scene
	HRESULT hresult = m_pDirect3dDevice->BeginScene();

	if (SUCCEEDED(hresult))
	{
		// Setup the world, view, and projection matrices
		CreateMatrices();

		// Meshes are divided into subsets, one for each material. Render them in
		// a loop
		for (DWORD i = 0; i < m_dwNumMaterials; i++)
		{
			// Set the material and texture for this subset
			m_pDirect3dDevice->SetMaterial(&m_pMeshMaterials[i]);
			m_pDirect3dDevice->SetTexture(0, m_pMeshTextures[i]);

			// Draw the mesh subset
			m_pMesh->DrawSubset(i);
		}

		// End the scene
		m_pDirect3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	m_pDirect3dDevice->Present(NULL, NULL, NULL, NULL);
}

void CRender::LoadScene()
{
	Log->Print("Loading scene...");

	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	HRESULT hresult = E_FAIL;

	hresult = D3DXLoadMeshFromX(L"..\\GameResources\\Tiger.x", D3DXMESH_SYSTEMMEM, m_pDirect3dDevice, NULL, &pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, &m_pMesh);

	if (FAILED(hresult))
	{
		// If model is not in current folder
		MessageBox(NULL, L"Could not find tiger.x", L"Meshes.exe", MB_OK);
		//return E_FAIL;
	}

	// We need to extract the material properties and texture names from the
	// pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
	if (m_pMeshMaterials == NULL)
		Log->Print("Can`t load mesh materials");
		//return E_OUTOFMEMORY;

	m_pMeshTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
	if (m_pMeshTextures == NULL)
		Log->Print("Can`t load mesh textures");
		//return E_OUTOFMEMORY;

	for (DWORD i = 0; i < m_dwNumMaterials; i++)
	{
		// Copy the material
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		m_pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL && lstrlenA(d3dxMaterials[i].pTextureFilename) > 0)
		{
			// Create the texture
			if (FAILED(D3DXCreateTextureFromFileA(m_pDirect3dDevice, d3dxMaterials[i].pTextureFilename, &m_pMeshTextures[i])))
			{
				// If texture is not in current folder, try parent folder
				const CHAR* strPrefix = "..\\GameResources\\";
				CHAR strTexture[MAX_PATH];
				strcpy_s(strTexture, MAX_PATH, strPrefix);
				strcat_s(strTexture, MAX_PATH, d3dxMaterials[i].pTextureFilename);
				// If texture is not in current folder, try parent folder
				if (FAILED(D3DXCreateTextureFromFileA(m_pDirect3dDevice, strTexture, &m_pMeshTextures[i])))
				{
					MessageBox(NULL, L"Could not find texture map", L"Meshes.exe", MB_OK);
				}
			}
		}
	}

	// Done with the material buffer
	pD3DXMtrlBuffer->Release();

	Log->Print("Scene loaded successfylly");
	//return S_OK;
}
///////////////////////////////////////////////////////////////
