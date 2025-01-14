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
	m_hWndow = NULL;
    m_pDirect3D = NULL;
    m_pDirect3dDevice = NULL;
}

CRender::~CRender()
{
    Log->Print("Destroying render...");

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
    m_hWndow = CreateWindow(L"ATLAS", L"Main window", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);
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
    HRESULT hresult = m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWndow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &Direct3DPresentParams, &m_pDirect3dDevice);

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

void CRender::Initialize()
{
    Log->Print("Initializing render...");
    CreateMainWindow();
    InitializeDirect3D();
}

void CRender::RenderFrame()
{
	Log->Print("Frame");
	Sleep(100);
}
///////////////////////////////////////////////////////////////
