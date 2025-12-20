#include "pch.h"
#include "Log.h"
#include "BackendDX11.h"
#include "Rendeructor.h"
#include <cstdio>
#include <string>
#include <vector>

struct SimpleVertex {
    float x, y, z;
    float u, v;
};

BackendDX11::BackendDX11() {
    LogDebug("[BackendDX11] Constructor called.");
}

BackendDX11::~BackendDX11() {
    Shutdown();
}

bool BackendDX11::Initialize(const BackendConfig& config) {
    LogDebug("[BackendDX11] Initializing...");

    if (!config.WindowHandle) {
        LogDebug("[BackendDX11] Error: WindowHandle is NULL");
        return false;
    }
    m_hwnd = (HWND)config.WindowHandle;

    m_screenWidth = config.Width;
    m_screenHeight = config.Height;

    if (!InitD3D(config)) {
        LogDebug("[BackendDX11] Error: InitD3D failed.");
        return false;
    }

    LogDebug("[BackendDX11] InitD3D success. Initializing Geometry...");
    InitQuadGeometry();

    LogDebug("[BackendDX11] Initialization Complete.");
    return true;
}

bool BackendDX11::InitD3D(const BackendConfig& config) {
    LogDebug("[BackendDX11] Setting up SwapChain...");

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = config.Width;
    scd.BufferDesc.Height = config.Height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Безопасная частота обновления
    scd.BufferDesc.RefreshRate.Numerator = 0;
    scd.BufferDesc.RefreshRate.Denominator = 1;

    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = m_hwnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE; // Принудительно оконный для теста
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    UINT flags = 0;
    //flags |= D3D11_CREATE_DEVICE_DEBUG;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL featureLevel;

    LogDebug("[BackendDX11] Calling D3D11CreateDeviceAndSwapChain...");

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
        featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &scd,
        m_swapChain.GetAddressOf(), m_device.GetAddressOf(), &featureLevel, m_context.GetAddressOf()
    );

    if (FAILED(hr)) {
        LogDebug("[BackendDX11] HARDWARE Creation FAILED! HRESULT: 0x%08X", (unsigned int)hr);
        return false;
    }
    else {
        LogDebug("[BackendDX11] Hardware Device Created successfully. Feature Level: 0x%X", featureLevel);
    }

    // RTV
    ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
    if (FAILED(hr)) {
        LogDebug("[BackendDX11] Failed to get backbuffer. HRESULT: 0x%08X", (unsigned int)hr);
        return false;
    }

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_backBufferRTV.GetAddressOf());
    if (FAILED(hr)) {
        LogDebug("[BackendDX11] Failed to create RTV. HRESULT: 0x%08X", (unsigned int)hr);
        return false;
    }

    // --- Создаем Rasterizer State (Отключаем Culling для теста) ---
    D3D11_RASTERIZER_DESC rd = {};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE; // Рисуем обе стороны граней
    rd.FrontCounterClockwise = FALSE;
    rd.DepthClipEnable = TRUE;

    hr = m_device->CreateRasterizerState(&rd, m_rasterizerState.GetAddressOf());
    if (FAILED(hr)) { LogDebug("Failed to create Rasterizer State"); return false; }

    m_context->RSSetState(m_rasterizerState.Get());

    // --- Создаем Depth Stencil State ---
    D3D11_DEPTH_STENCIL_DESC dsdSetup = {};
    dsdSetup.DepthEnable = TRUE;
    dsdSetup.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsdSetup.DepthFunc = D3D11_COMPARISON_LESS; // Стандартная проверка глубины

    hr = m_device->CreateDepthStencilState(&dsdSetup, m_depthStencilState.GetAddressOf());
    if (FAILED(hr)) { LogDebug("Failed to create Depth State"); return false; }

    m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

    // Создаем сам буфер глубины
    CreateDepthResources(config.Width, config.Height);

    // 1. Default (Write On, Test On) - уже было, сохраним в m_dssDefault
    D3D11_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthEnable = TRUE;
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc = D3D11_COMPARISON_LESS;
    m_device->CreateDepthStencilState(&dsd, m_dssDefault.GetAddressOf());

    // 2. No Write (Write Off, Test On/Off) - для Скайбокса или прозрачности
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Не пишем в глубину
    dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Чтобы скайбокс рисовался на фоне
    m_device->CreateDepthStencilState(&dsd, m_dssNoWrite.GetAddressOf());

    // Ставим дефолт
    m_context->OMSetDepthStencilState(m_dssDefault.Get(), 0);

    LogDebug("[BackendDX11] Initializing Viewport...");
    Resize(config.Width, config.Height);

    LogDebug("[BackendDX11] Initializing Render States...");
    InitRenderStates();

    SetRenderTarget(nullptr);

    return true;
}

void BackendDX11::InitRenderStates() {
    D3D11_RASTERIZER_DESC rd = {};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.DepthClipEnable = TRUE;
    rd.FrontCounterClockwise = FALSE; // false = clockwise (стандарт DX)
    rd.MultisampleEnable = FALSE;
    rd.AntialiasedLineEnable = FALSE;

    // Массив режимов соответствует enum CullMode { None=0, Front=1, Back=2 }
    D3D11_CULL_MODE cullTranslation[3] = { D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK };

    for (int i = 0; i < 3; i++) {
        rd.CullMode = cullTranslation[i];

        // Вариант 0: Scissor ВЫКЛ
        rd.ScissorEnable = FALSE;
        HRESULT hr1 = m_device->CreateRasterizerState(&rd, m_rasterizerStates[i][0].GetAddressOf());

        // Вариант 1: Scissor ВКЛ
        rd.ScissorEnable = TRUE;
        HRESULT hr2 = m_device->CreateRasterizerState(&rd, m_rasterizerStates[i][1].GetAddressOf());

        if (FAILED(hr1) || FAILED(hr2)) {
            LogDebug("[BackendDX11] Error creating Rasterizer State %d", i);
        }
    }

    // 1. Устанавливаем переменные кэша в то, что мы хотим видеть по умолчанию
    m_cachedCullMode = CullMode::Back;     // По умолчанию Cull Back
    m_cachedScissorEnabled = false;        // По умолчанию Scissor Off

    // 2. Явно устанавливаем этот стейт в GPU (индекс 2 = Back, индекс 0 = ScissorOff)
    if (m_context) {
        m_context->RSSetState(m_rasterizerStates[(int)m_cachedCullMode][0].Get());
    }

    // Инициализация состояний смешивания (Blend)
    D3D11_BLEND_DESC bd = {};
    bd.RenderTarget[0].BlendEnable = FALSE;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    m_device->CreateBlendState(&bd, m_blendStates[0].GetAddressOf()); // Opaque

    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    m_device->CreateBlendState(&bd, m_blendStates[1].GetAddressOf()); // Alpha

    m_cachedBlendMode = BlendMode::Opaque;
    float factor[4] = { 0,0,0,0 };
    m_context->OMSetBlendState(m_blendStates[0].Get(), factor, 0xFFFFFFFF);
}

void BackendDX11::SetPipelineState(const PipelineState& newState) {
    // 1. Проверка изменений для Rasterizer State (Cull + Scissor)
    bool rasterizerDirty = m_firstStateSet ||
        (newState.Cull != m_activeState.Cull) ||
        (newState.ScissorTest != m_activeState.ScissorTest);

    if (rasterizerDirty) {
        int cullIndex = (int)newState.Cull;
        // Защита от выхода за массив
        if (cullIndex < 0 || cullIndex > 2) cullIndex = 2; // Default Back

        // Индекс 1, если сциссор включен, 0 - выключен
        int scissorIndex = newState.ScissorTest ? 1 : 0;

        // Используем твой уже существующий массив предсозданных стейтов
        if (m_rasterizerStates[cullIndex][scissorIndex]) {
            m_context->RSSetState(m_rasterizerStates[cullIndex][scissorIndex].Get());
        }
    }

    // 2. Проверка изменений для Blend State
    if (m_firstStateSet || (newState.Blend != m_activeState.Blend)) {
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        int blendIdx = (int)newState.Blend;
        if (blendIdx >= 0 && blendIdx <= 2) {
            m_context->OMSetBlendState(m_blendStates[blendIdx].Get(), blendFactor, 0xffffffff);
        }
    }

    // 3. Проверка изменений для Depth State
    if (m_firstStateSet ||
        (newState.DepthFunc != m_activeState.DepthFunc) ||
        (newState.DepthWrite != m_activeState.DepthWrite))
    {
        // Используем твою существующую функцию GetDepthState с map'ом
        ID3D11DepthStencilState* dsState = GetDepthState(newState.DepthFunc, newState.DepthWrite);
        if (dsState) {
            // stensilRef = 1 (дефолт)
            m_context->OMSetDepthStencilState(dsState, 1);
        }
    }

    // Обновляем кэш
    m_activeState = newState;
    m_firstStateSet = false;
}

void BackendDX11::SetScissorRect(int x, int y, int width, int height) {
    D3D11_RECT rects[1];
    rects[0].left = x;
    rects[0].top = y;
    rects[0].right = x + width;
    rects[0].bottom = y + height;
    m_context->RSSetScissorRects(1, rects);
}

ID3D11DepthStencilState* BackendDX11::GetDepthState(CompareFunc func, bool write) {
    // Генерируем уникальный ключ для map
    // func (3 бита) | write (1 бит)
    uint32_t key = ((uint32_t)func << 1) | (write ? 1 : 0);

    // Если уже есть - возвращаем
    if (m_depthStates.find(key) != m_depthStates.end()) {
        return m_depthStates[key].Get();
    }

    // Если нет - создаем
    D3D11_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthEnable = TRUE;
    dsd.DepthWriteMask = write ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

    switch (func) {
    case CompareFunc::Never:        dsd.DepthFunc = D3D11_COMPARISON_NEVER; break;
    case CompareFunc::Less:         dsd.DepthFunc = D3D11_COMPARISON_LESS; break;
    case CompareFunc::Equal:        dsd.DepthFunc = D3D11_COMPARISON_EQUAL; break;
    case CompareFunc::LessEqual:    dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; break;
    case CompareFunc::Greater:      dsd.DepthFunc = D3D11_COMPARISON_GREATER; break;
    case CompareFunc::NotEqual:     dsd.DepthFunc = D3D11_COMPARISON_NOT_EQUAL; break;
    case CompareFunc::GreaterEqual: dsd.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL; break;
    case CompareFunc::Always:       dsd.DepthFunc = D3D11_COMPARISON_ALWAYS; break;
    default:                        dsd.DepthFunc = D3D11_COMPARISON_LESS; break;
    }

    // Stencil выключаем для простоты
    dsd.StencilEnable = FALSE;

    m_device->CreateDepthStencilState(&dsd, m_depthStates[key].GetAddressOf());
    return m_depthStates[key].Get();
}

void BackendDX11::CreateDepthResources(int width, int height) {
    m_depthStencilBuffer.Reset();
    m_depthStencilView.Reset();

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bit depth, 8 bit stencil
    descDepth.SampleDesc.Count = 1;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    if (FAILED(m_device->CreateTexture2D(&descDepth, nullptr, m_depthStencilBuffer.GetAddressOf()))) return;
    if (FAILED(m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_depthStencilView.GetAddressOf()))) return;
}

void BackendDX11::Shutdown() {
    LogDebug("[BackendDX11] Shutdown called.");
    m_depthCache.clear();
    for (auto* t : m_textures) delete t;
    m_textures.clear();
    for (auto* s : m_samplers) delete s;
    m_samplers.clear();
    m_shaderCache.clear();
}

void BackendDX11::Resize(int width, int height) {
    m_screenWidth = width;
    m_screenHeight = height;
    if (!m_context) return;

    m_depthCache.clear();

    CreateDepthResources(width, height); // Пересоздаем глубину

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_context->RSSetViewports(1, &vp);
}

void BackendDX11::BeginFrame() {}

void BackendDX11::EndFrame() {
    if (m_swapChain) m_swapChain->Present(1, 0);
}

void* BackendDX11::CreateTextureResource(int width, int height, int format, const void* initialData) {
    auto* wrapper = new DX11TextureWrapper();
    wrapper->Width = width;
    wrapper->Height = height;
    wrapper->Type = TextureType::Tex2D;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    // Определяем формат и размер пикселя для загрузки данных
    int bytesPerPixel = 4;
    switch ((TextureFormat)format) {
    case TextureFormat::RGBA16F:
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        bytesPerPixel = 8;
        break;
    case TextureFormat::R16F:
        desc.Format = DXGI_FORMAT_R16_FLOAT;
        bytesPerPixel = 2;
        break;
    case TextureFormat::R32F:
        desc.Format = DXGI_FORMAT_R32_FLOAT;
        bytesPerPixel = 4;
        break;
    default:
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        bytesPerPixel = 4;
        break;
    }

    D3D11_SUBRESOURCE_DATA* pInitData = nullptr;
    D3D11_SUBRESOURCE_DATA initData = {};

    if (initialData) {
        initData.pSysMem = initialData;
        initData.SysMemPitch = width * bytesPerPixel; // Шаг строки
        pInitData = &initData;
    }

    HRESULT hr = m_device->CreateTexture2D(&desc, pInitData, wrapper->Texture.GetAddressOf());

    if (FAILED(hr)) {
        LogDebug("[BackendDX11] Failed create texture. Hr: 0x%X", hr);
        delete wrapper;
        return nullptr;
    }

    m_device->CreateShaderResourceView(wrapper->Texture.Get(), nullptr, wrapper->SRV.GetAddressOf());
    m_device->CreateRenderTargetView(wrapper->Texture.Get(), nullptr, wrapper->RTV.GetAddressOf());

    m_textures.push_back(wrapper);
    return wrapper;
}

void* BackendDX11::CreateTextureCubeResource(int width, int height, int format, const void** initialData) {
    auto* wrapper = new DX11TextureWrapper();
    wrapper->Width = width;
    wrapper->Height = height;
    wrapper->Type = TextureType::TexCube;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 6; // ВАЖНО: 6 граней
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Пока хардкодим RGBA8
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT; // Изменим на IMMUTABLE, так как скайбокс статичен
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // ВАЖНО: Говорим, что это КУБ

    // Подготовка данных для 6 граней
    D3D11_SUBRESOURCE_DATA subData[6];
    if (initialData) {
        for (int i = 0; i < 6; i++) {
            subData[i].pSysMem = initialData[i];
            subData[i].SysMemPitch = width * 4; // 4 байта на пиксель (RGBA8)
            subData[i].SysMemSlicePitch = 0;
        }
    }

    if (FAILED(m_device->CreateTexture2D(&desc, initialData ? subData : nullptr, wrapper->Texture.GetAddressOf()))) {
        delete wrapper;
        return nullptr;
    }

    // Создание SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // ВАЖНО: Вид как куб
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.MipLevels = 1;

    if (FAILED(m_device->CreateShaderResourceView(wrapper->Texture.Get(), &srvDesc, wrapper->SRV.GetAddressOf()))) {
        delete wrapper;
        return nullptr;
    }

    m_textures.push_back(wrapper);
    return wrapper;
}

void BackendDX11::CopyTexture(void* dstHandle, void* srcHandle) {
    if (!dstHandle || !srcHandle) return;
    auto* dst = (DX11TextureWrapper*)dstHandle;
    auto* src = (DX11TextureWrapper*)srcHandle;

    // Копирует все содержимое (размеры должны совпадать, иначе DX выдаст ошибку в debug layer)
    m_context->CopyResource(dst->Texture.Get(), src->Texture.Get());
}

void BackendDX11::SetRenderTarget(void* target1, void* target2,
    void* target3, void* target4) {
    // Собираем все ненулевые цели
    ID3D11RenderTargetView* rtvs[4] = { nullptr, nullptr, nullptr, nullptr };
    int count = 0;

    auto addTarget = [&](void* handle) {
        if (handle) {
            auto* tex = (DX11TextureWrapper*)handle;
            if (tex->RTV) {
                rtvs[count++] = tex->RTV.Get();
            }
        }
    };

    addTarget(target1);
    addTarget(target2);
    addTarget(target3);
    addTarget(target4);

    SetRenderTargetsInternal(rtvs, count);
}

// Хелпер для очистки конкретного RTV
void BackendDX11::ClearRTV(ID3D11RenderTargetView* rtv, float r, float g, float b, float a) {
    if (rtv) {
        float color[4] = { r, g, b, a };
        m_context->ClearRenderTargetView(rtv, color);
    }
}

inline uint64_t PackSize(int w, int h) {
    return ((uint64_t)w << 32) | (uint64_t)h;
}

ID3D11DepthStencilView* BackendDX11::GetDepthStencilForSize(int width, int height) {
    // 1. Проверяем, может это основной экран?
    if (width == m_screenWidth && height == m_screenHeight) {
        return m_depthStencilView.Get(); // Возвращаем главный буфер окна
    }

    // 2. Ищем в кэше
    uint64_t key = PackSize(width, height);
    auto it = m_depthCache.find(key);
    if (it != m_depthCache.end()) {
        return it->second.DSV.Get();
    }

    // 3. Не нашли - создаем новый
    LogDebug("[BackendDX11] Creating new auto-depth buffer for resolution %dx%d", width, height);

    DepthBufferCacheItem item;

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    // D24_S8 - самый совместимый формат
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    HRESULT hr = m_device->CreateTexture2D(&descDepth, nullptr, item.Texture.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    hr = m_device->CreateDepthStencilView(item.Texture.Get(), nullptr, item.DSV.GetAddressOf());
    if (FAILED(hr)) return nullptr;

    // Сохраняем в кэш
    m_depthCache[key] = item;
    return item.DSV.Get();
}

void BackendDX11::SetRenderTargetsInternal(ID3D11RenderTargetView* rtvs[], int count) {
    UnbindResources();
    m_boundRTVs.clear();

    ID3D11DepthStencilView* dsvToBind = nullptr;
    int targetW = m_screenWidth;
    int targetH = m_screenHeight;

    if (count > 0 && rtvs[0] != nullptr) {
        ID3D11Resource* res = nullptr;
        rtvs[0]->GetResource(&res);
        D3D11_TEXTURE2D_DESC desc;
        ((ID3D11Texture2D*)res)->GetDesc(&desc);
        res->Release(); // Важно релизить COM объект после Get

        targetW = desc.Width;
        targetH = desc.Height;

        dsvToBind = GetDepthStencilForSize(targetW, targetH);

        m_context->OMSetRenderTargets(count, rtvs, dsvToBind);

        for (int i = 0; i < count; i++) {
            if (rtvs[i]) m_boundRTVs.push_back(rtvs[i]);
        }
    }
    else {
        dsvToBind = m_depthStencilView.Get(); // Главный Z-буфер
        ID3D11RenderTargetView* bb = m_backBufferRTV.Get();

        m_context->OMSetRenderTargets(1, &bb, dsvToBind);
        m_boundRTVs.push_back(bb);

        targetW = m_screenWidth;
        targetH = m_screenHeight;
    }

    m_currentDSV = dsvToBind;

    D3D11_VIEWPORT vp = {};
    vp.Width = (float)targetW;
    vp.Height = (float)targetH;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    m_context->RSSetViewports(1, &vp);
}

void BackendDX11::Clear(float r, float g, float b, float a) {
    for (auto* rtv : m_boundRTVs) {
        ClearRTV(rtv, r, g, b, a);
    }

    if (m_currentDSV) {
        m_context->ClearDepthStencilView(m_currentDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

void BackendDX11::ClearTexture(void* textureHandle, float r, float g, float b, float a) {
    if (!textureHandle) return;
    auto* tex = (DX11TextureWrapper*)textureHandle;

    if (tex && tex->RTV) {
        ClearRTV(tex->RTV.Get(), r, g, b, a);
    }
}

void BackendDX11::ClearDepth(float depth, int stencil) {
    if (m_currentDSV) {
        m_context->ClearDepthStencilView(m_currentDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, (UINT8)stencil);
    }
}

void* BackendDX11::CreateTexture3DResource(int width, int height, int depth, int format, const void* initialData) {
    auto* wrapper = new DX11TextureWrapper();
    wrapper->Width = width; wrapper->Height = height; wrapper->Depth = depth;
    wrapper->Type = TextureType::Tex3D;

    D3D11_TEXTURE3D_DESC desc = {};
    desc.Width = width; desc.Height = height; desc.Depth = depth;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // Для теста градиента используем float4
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // Подготовка данных
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = initialData;
    initData.SysMemPitch = width * sizeof(float) * 4; // Строка
    initData.SysMemSlicePitch = width * height * sizeof(float) * 4; // Слой

    if (FAILED(m_device->CreateTexture3D(&desc, &initData, wrapper->Texture3D.GetAddressOf()))) {
        delete wrapper; return nullptr;
    }

    // Создаем SRV
    if (FAILED(m_device->CreateShaderResourceView(wrapper->Texture3D.Get(), nullptr, wrapper->SRV.GetAddressOf()))) {
        delete wrapper; return nullptr;
    }

    m_textures.push_back(wrapper);
    return wrapper;
}

void* BackendDX11::CreateSamplerResource(const std::string& filterMode) {
    auto* wrapper = new DX11SamplerWrapper();
    D3D11_SAMPLER_DESC desc = {};
    desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    if (filterMode == "Point") desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    else desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

    m_device->CreateSamplerState(&desc, wrapper->State.GetAddressOf());
    m_samplers.push_back(wrapper);
    return wrapper;
}

void BackendDX11::InitQuadGeometry() {
    SimpleVertex vertices[] = {
        { -1.0f, -1.0f, 0.0f,  0.0f, 1.0f },
        { -1.0f,  1.0f, 0.0f,  0.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f,  1.0f, 1.0f },
        {  1.0f,  1.0f, 0.0f,  1.0f, 0.0f },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    m_device->CreateBuffer(&bd, &initData, m_quadVertexBuffer.GetAddressOf());

    unsigned long indices[] = { 0, 1, 2, 2, 1, 3 };
    bd.ByteWidth = sizeof(unsigned long) * 6;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = indices;
    m_device->CreateBuffer(&bd, &initData, m_quadIndexBuffer.GetAddressOf());
}

bool BackendDX11::CompileShader(const std::string& path, const std::string& entry, const std::string& profile, ID3DBlob** outBlob) {
    std::wstring wpath(path.begin(), path.end());
    ID3DBlob* errorBlob = nullptr;
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = D3DCompileFromFile(wpath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), profile.c_str(), flags, 0, outBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            LogDebug("[Shader Error] %s", (char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        else {
            LogDebug("[Shader Error] Failed to find file: %s", path.c_str());
        }
        return false;
    }
    return true;
}

DX11ReflectionData BackendDX11::ReflectShader(ID3DBlob* blob) {
    DX11ReflectionData data;

    ComPtr<ID3D11ShaderReflection> reflector;
    if (FAILED(D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)reflector.GetAddressOf()))) {
        return data;
    }

    D3D11_SHADER_DESC shaderDesc;
    reflector->GetDesc(&shaderDesc);

    // 1. Сначала собираем информацию о слотах (Bind Points)
    std::map<std::string, UINT> cbSlots;

    for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
        D3D11_SHADER_INPUT_BIND_DESC bindDesc;
        reflector->GetResourceBindingDesc(i, &bindDesc);
        std::string name = bindDesc.Name;

        if (bindDesc.Type == D3D_SIT_CBUFFER) {
            cbSlots[name] = bindDesc.BindPoint;
        }
        else if (bindDesc.Type == D3D_SIT_TEXTURE) {
            data.TextureSlots[name] = bindDesc.BindPoint;
        }
        else if (bindDesc.Type == D3D_SIT_SAMPLER) {
            data.SamplerSlots[name] = bindDesc.BindPoint;
        }
    }

    // 2. Теперь читаем содержимое буферов и создаем наши структуры
    for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i) {
        ID3D11ShaderReflectionConstantBuffer* cb = reflector->GetConstantBufferByIndex(i);
        D3D11_SHADER_BUFFER_DESC bufferDesc;
        cb->GetDesc(&bufferDesc);

        ReflectedConstantBuffer myCB;
        myCB.Name = bufferDesc.Name;
        myCB.Size = bufferDesc.Size;

        // Находим слот, который мы сохранили на шаге 1
        if (cbSlots.find(myCB.Name) != cbSlots.end()) {
            myCB.Slot = cbSlots[myCB.Name];
        }
        else {
            myCB.Slot = i; // Fallback, если что-то пошло не так
        }

        // Читаем переменные внутри этого буфера
        for (UINT j = 0; j < bufferDesc.Variables; ++j) {
            ID3D11ShaderReflectionVariable* var = cb->GetVariableByIndex(j);
            D3D11_SHADER_VARIABLE_DESC varDesc;
            var->GetDesc(&varDesc);

            ConstantBufferVariable v;
            v.Name = varDesc.Name;
            v.Offset = varDesc.StartOffset;
            v.Size = varDesc.Size;
            myCB.Variables.push_back(v);
        }

        data.Buffers.push_back(myCB);
    }

    return data;
}

void BackendDX11::PrepareShaderPass(const ShaderPass& pass) {
    std::string key = pass.VertexShaderPath + ":" + pass.VertexShaderEntryPoint + "|" + pass.PixelShaderPath + ":" + pass.PixelShaderEntryPoint;
    if (m_shaderCache.find(key) != m_shaderCache.end()) return;

    LogDebug("[BackendDX11] Compiling Shader Pass: %s", key.c_str());

    DX11ShaderWrapper sw;
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    // --- VERTEX SHADER ---
    if (CompileShader(pass.VertexShaderPath, pass.VertexShaderEntryPoint, "vs_5_0", &vsBlob)) {
        m_device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, sw.VertexShader.GetAddressOf());
        sw.ReflectionVS = ReflectShader(vsBlob);

        // Создаем буферы для VS
        for (auto& cb : sw.ReflectionVS.Buffers) {
            D3D11_BUFFER_DESC bd = {};
            bd.ByteWidth = (cb.Size + 15) / 16 * 16;
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            m_device->CreateBuffer(&bd, nullptr, cb.HardwareBuffer.GetAddressOf());
            cb.ShadowData.resize(bd.ByteWidth, 0);
        }

        std::vector<char> bytecode((char*)vsBlob->GetBufferPointer(), (char*)vsBlob->GetBufferPointer() + vsBlob->GetBufferSize());
        CreateInputLayoutFromShader(bytecode, sw.InputLayout.GetAddressOf());

        vsBlob->Release();
    }

    // --- PIXEL SHADER ---
    if (CompileShader(pass.PixelShaderPath, pass.PixelShaderEntryPoint, "ps_5_0", &psBlob)) {
        m_device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, sw.PixelShader.GetAddressOf());
        sw.ReflectionPS = ReflectShader(psBlob);

        for (auto& cb : sw.ReflectionPS.Buffers) {
            D3D11_BUFFER_DESC bd = {};
            bd.ByteWidth = (cb.Size + 15) / 16 * 16;
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            m_device->CreateBuffer(&bd, nullptr, cb.HardwareBuffer.GetAddressOf());
            cb.ShadowData.resize(bd.ByteWidth, 0);
        }

        psBlob->Release();
    }

    m_shaderCache[key] = sw;
}

void BackendDX11::SetShaderPass(const ShaderPass& pass) {
    // 1. Генерируем ключ для поиска в кэше
    std::string key = pass.VertexShaderPath + ":" + pass.VertexShaderEntryPoint + "|" +
        pass.PixelShaderPath + ":" + pass.PixelShaderEntryPoint;

    // Если шейдер не скомпилирован или не найден — выходим
    if (m_shaderCache.find(key) == m_shaderCache.end()) return;

    // Устанавливаем активный шейдер
    m_activeShader = &m_shaderCache[key];

    // 2. Устанавливаем пайплайн (InputLayout, VS, PS)
    m_context->IASetInputLayout(m_activeShader->InputLayout.Get());
    m_context->VSSetShader(m_activeShader->VertexShader.Get(), nullptr, 0);
    m_context->PSSetShader(m_activeShader->PixelShader.Get(), nullptr, 0);

    // 3. Привязка 2D Текстур по имени (используя данные рефлексии)
    for (const auto& texPair : pass.GetTextures()) {
        const std::string& name = texPair.first;
        const Texture* texturePtr = texPair.second;

        // Получаем внутренний хендл
        auto* tex = (DX11TextureWrapper*)texturePtr->GetHandle();

        if (tex) {
            // Если текстура используется в Pixel Shader
            if (m_activeShader->ReflectionPS.TextureSlots.count(name)) {
                UINT slot = m_activeShader->ReflectionPS.TextureSlots[name];
                m_context->PSSetShaderResources(slot, 1, tex->SRV.GetAddressOf());
            }
            // Если текстура используется в Vertex Shader
            if (m_activeShader->ReflectionVS.TextureSlots.count(name)) {
                UINT slot = m_activeShader->ReflectionVS.TextureSlots[name];
                m_context->VSSetShaderResources(slot, 1, tex->SRV.GetAddressOf());
            }
        }
    }

    // 4. Привязка 3D Текстур по имени
    for (const auto& tex3DPair : pass.GetTextures3D()) {
        const std::string& name = tex3DPair.first;
        const Texture3D* texturePtr = tex3DPair.second;
        auto* tex = (DX11TextureWrapper*)texturePtr->GetHandle();

        if (tex && tex->SRV) {
            // Проверка для Pixel Shader (PS)
            if (m_activeShader->ReflectionPS.TextureSlots.count(name)) {
                UINT slot = m_activeShader->ReflectionPS.TextureSlots[name];
                m_context->PSSetShaderResources(slot, 1, tex->SRV.GetAddressOf());
            }

            // Проверка для Vertex Shader (VS)
            if (m_activeShader->ReflectionVS.TextureSlots.count(name)) {
                UINT slot = m_activeShader->ReflectionVS.TextureSlots[name];
                m_context->VSSetShaderResources(slot, 1, tex->SRV.GetAddressOf());
            }
        }
    }

    // 5. Привязка Семплеров по имени
    for (const auto& sampPair : pass.GetSamplers()) {
        const std::string& name = sampPair.first;
        const Sampler* samplerPtr = sampPair.second;
        auto* smp = (DX11SamplerWrapper*)samplerPtr->GetHandle();

        if (smp) {
            // Pixel Shader
            if (m_activeShader->ReflectionPS.SamplerSlots.count(name)) {
                UINT slot = m_activeShader->ReflectionPS.SamplerSlots[name];
                m_context->PSSetSamplers(slot, 1, smp->State.GetAddressOf());
            }
            // Vertex Shader
            if (m_activeShader->ReflectionVS.SamplerSlots.count(name)) {
                UINT slot = m_activeShader->ReflectionVS.SamplerSlots[name];
                m_context->VSSetSamplers(slot, 1, smp->State.GetAddressOf());
            }
        }
    }

    // 6. Привязка TextureCube (Cubemaps)
    for (const auto& texCubePair : pass.GetTexturesCube()) {
        const std::string& name = texCubePair.first;
        const TextureCube* texturePtr = texCubePair.second;
        auto* tex = (DX11TextureWrapper*)texturePtr->GetHandle();

        if (tex && tex->SRV && tex->Type == TextureType::TexCube) {
            // Pixel Shader
            if (m_activeShader->ReflectionPS.TextureSlots.count(name)) {
                UINT slot = m_activeShader->ReflectionPS.TextureSlots[name];
                m_context->PSSetShaderResources(slot, 1, tex->SRV.GetAddressOf());
            }
            // Vertex Shader
            if (m_activeShader->ReflectionVS.TextureSlots.count(name)) {
                UINT slot = m_activeShader->ReflectionVS.TextureSlots[name];
                m_context->VSSetShaderResources(slot, 1, tex->SRV.GetAddressOf());
            }
        }
    }

}

void BackendDX11::UpdateConstantRaw(const std::string& name, const void* data, size_t size) {
    std::vector<uint8_t> buffer(size);
    memcpy(buffer.data(), data, size);
    m_cpuConstantsStorage[name] = { buffer };
}

void BackendDX11::UploadConstants(DX11ReflectionData& reflectionData, ShaderType SType) {
    for (auto& cb : reflectionData.Buffers) {
        if (!cb.HardwareBuffer) continue;

        bool isDirty = false;

        // 1. Проверяем, обновил ли пользователь ВЕСЬ буфер целиком по имени
        // (Например: UpdateConstantRaw("SSAOConfigBuffer", &data, size))
        if (m_cpuConstantsStorage.count(cb.Name)) {
            const auto& stored = m_cpuConstantsStorage[cb.Name];
            if (stored.Data.size() <= cb.ShadowData.size()) {
                memcpy(cb.ShadowData.data(), stored.Data.data(), stored.Data.size());
                isDirty = true;
                // Важно: Мы не удаляем данные из map, так как они могут понадобиться в следующем кадре,
                // но можно оптимизировать флагами "dirty".
            }
        }

        // 2. Проверяем отдельные переменные внутри буфера
        // (Например: SetConstant("World", ...))
        for (const auto& var : cb.Variables) {
            if (m_cpuConstantsStorage.count(var.Name)) {
                const auto& stored = m_cpuConstantsStorage[var.Name];

                // Проверка выхода за границы
                if (var.Offset + var.Size <= cb.ShadowData.size() &&
                    stored.Data.size() >= var.Size)
                {
                    // Копируем в теневой буфер
                    memcpy(cb.ShadowData.data() + var.Offset, stored.Data.data(), var.Size);
                    isDirty = true;
                }
            }
        }

        // 3. Заливаем в GPU (Всегда заливаем всё, Map_DISCARD требует этого!)
        // Даже если isDirty == false, данные могли быть нужны с прошлого кадра, 
        // но DX11 требует, чтобы мы обновляли Dynamic буферы или не трогали их. 
        // В нашем случае, так как мы используем одну карту storage на все шейдеры,
        // лучше обновлять.

        D3D11_MAPPED_SUBRESOURCE map;
        if (SUCCEEDED(m_context->Map(cb.HardwareBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map))) {
            memcpy(map.pData, cb.ShadowData.data(), cb.ShadowData.size());
            m_context->Unmap(cb.HardwareBuffer.Get(), 0);
        }

        // 4. Биндим буфер
        if (SType == ShaderType::Vertex) {
            m_context->VSSetConstantBuffers(cb.Slot, 1, cb.HardwareBuffer.GetAddressOf());
        }
        else {
            m_context->PSSetConstantBuffers(cb.Slot, 1, cb.HardwareBuffer.GetAddressOf());
        }
    }
}

void BackendDX11::UnbindResources() {
    ID3D11ShaderResourceView* nullSRVs[16] = { nullptr };

    if (m_context) {
        m_context->PSSetShaderResources(0, 16, nullSRVs);
        m_context->VSSetShaderResources(0, 16, nullSRVs);
    }
}

void BackendDX11::CreateInputLayoutFromShader(const std::vector<char>& shaderBytecode, ID3D11InputLayout** outLayout) {
    ID3D11ShaderReflection* reflector = nullptr;
    if (FAILED(D3DReflect(shaderBytecode.data(), shaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&reflector))) {
        return;
    }

    D3D11_SHADER_DESC shaderDesc;
    reflector->GetDesc(&shaderDesc);

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

    for (UINT i = 0; i < shaderDesc.InputParameters; i++) {
        D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
        reflector->GetInputParameterDesc(i, &paramDesc);

        D3D11_INPUT_ELEMENT_DESC element = {};
        element.SemanticName = paramDesc.SemanticName;
        element.SemanticIndex = paramDesc.SemanticIndex;
        element.Format = DXGI_FORMAT_UNKNOWN; // Будет определено ниже по маске
        element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

        // --- ЛОГИКА ИНСТАНСИНГА ---
        std::string semanticName = paramDesc.SemanticName;
        // Если семантика начинается с "INSTANCE_", считаем это данными инстанса (Slot 1)
        if (semanticName.rfind("INSTANCE_", 0) == 0) {
            element.InputSlot = 1; // Instance Buffer Slot
            element.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
            element.InstanceDataStepRate = 1; // 1 шаг на 1 инстанс
        }
        else {
            element.InputSlot = 0; // Vertex Buffer Slot
            element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            element.InstanceDataStepRate = 0;
        }

        // Определение формата (упрощенное, но рабочее для float)
        if (paramDesc.Mask == 1) element.Format = DXGI_FORMAT_R32_FLOAT;
        else if (paramDesc.Mask <= 3) element.Format = DXGI_FORMAT_R32G32_FLOAT;
        else if (paramDesc.Mask <= 7) element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        else if (paramDesc.Mask <= 15) element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        inputLayoutDesc.push_back(element);
    }

    m_device->CreateInputLayout(inputLayoutDesc.data(), (UINT)inputLayoutDesc.size(), shaderBytecode.data(), shaderBytecode.size(), outLayout);
    reflector->Release();
}

void BackendDX11::DrawFullScreenQuad() {
    if (!m_activeShader) return;

    UploadConstants(m_activeShader->ReflectionVS, ShaderType::Vertex);
    UploadConstants(m_activeShader->ReflectionPS, ShaderType::Pixel);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, m_quadVertexBuffer.GetAddressOf(), &stride, &offset);
    m_context->IASetIndexBuffer(m_quadIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_context->DrawIndexed(6, 0, 0);

    // Очистка SRV (чтобы можно было писать в эти текстуры на след. кадре)
    ID3D11ShaderResourceView* nullSRVs[8] = { nullptr };
    m_context->PSSetShaderResources(0, 8, nullSRVs);
}

void* BackendDX11::CreateBufferInternal(const void* data, size_t size, UINT bindFlags) {
    auto* wrapper = new DX11BufferWrapper();
    wrapper->Size = (UINT)size;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = (UINT)size;
    bd.BindFlags = bindFlags;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;

    HRESULT hr = m_device->CreateBuffer(&bd, &initData, wrapper->Buffer.GetAddressOf());
    if (FAILED(hr)) {
        LogDebug("[BackendDX11] Failed to create buffer. Hr: 0x%X", hr);
        delete wrapper;
        return nullptr;
    }
    return wrapper;
}

void* BackendDX11::CreateVertexBuffer(const void* data, size_t size, int stride) {
    auto* w = (DX11BufferWrapper*)CreateBufferInternal(data, size, D3D11_BIND_VERTEX_BUFFER);

    if (w) {
        w->Stride = (UINT)stride;
    }

    return w;
}

void* BackendDX11::CreateIndexBuffer(const void* data, size_t size) {
    return CreateBufferInternal(data, size, D3D11_BIND_INDEX_BUFFER);
}

void* BackendDX11::CreateInstanceBuffer(const void* data, size_t size, int stride) {
    auto* w = (DX11BufferWrapper*)CreateBufferInternal(data, size, D3D11_BIND_VERTEX_BUFFER);
    if (w) {
        w->Stride = (UINT)stride;
    }
    return w;
}

void BackendDX11::DrawMesh(void* vbHandle, void* ibHandle, int indexCount) {
    // Базовые проверки
    if (!m_activeShader || !vbHandle || !ibHandle) return;

    // Приведение типов к нашим внутренним оберткам
    auto* vb = (DX11BufferWrapper*)vbHandle;
    auto* ib = (DX11BufferWrapper*)ibHandle;

    // 1. Обновляем и биндим константы для Vertex Shader (поддержка мульти-буферов)
    UploadConstants(m_activeShader->ReflectionVS, ShaderType::Vertex);

    // 2. Обновляем и биндим константы для Pixel Shader
    UploadConstants(m_activeShader->ReflectionPS, ShaderType::Pixel);

    // -----------------------------------------------------------
    // 3. Установка геометрии (Input Assembler)
    // -----------------------------------------------------------
    UINT stride = vb->Stride; // Размер одной вершины (шаг)
    UINT offset = 0;

    // Устанавливаем Вершинный Буфер
    m_context->IASetVertexBuffers(0, 1, vb->Buffer.GetAddressOf(), &stride, &offset);

    // Устанавливаем Индексный Буфер (Формат R32_UINT означает unsigned int)
    m_context->IASetIndexBuffer(ib->Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // Указываем тип примитивов (список треугольников)
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // -----------------------------------------------------------
    // 4. Отрисовка
    // -----------------------------------------------------------
    m_context->DrawIndexed(indexCount, 0, 0);

    // -----------------------------------------------------------
    // 5. Очистка ресурсов
    // -----------------------------------------------------------
    // Важно сбросить SRV, чтобы избежать конфликтов чтения/записи в следующих проходах
    ID3D11ShaderResourceView* nullSRVs[8] = { nullptr };
    m_context->PSSetShaderResources(0, 8, nullSRVs);
}

void BackendDX11::DrawMeshInstanced(void* vbHandle, void* ibHandle, int indexCount, void* instHandle, int instanceCount, int instanceStride) {
    if (!m_activeShader || !vbHandle || !ibHandle || !instHandle) return;

    auto* vb = (DX11BufferWrapper*)vbHandle;
    auto* ib = (DX11BufferWrapper*)ibHandle;
    auto* instBuffer = (DX11BufferWrapper*)instHandle;

    // 1. Константы
    UploadConstants(m_activeShader->ReflectionVS, ShaderType::Vertex);
    UploadConstants(m_activeShader->ReflectionPS, ShaderType::Pixel);

    // 2. Установка буферов
    // Slot 0: Геометрия (Mesh)
    // Slot 1: Инстанс данные (Transform matrix, color, id etc)
    ID3D11Buffer* vbs[] = { vb->Buffer.Get(), instBuffer->Buffer.Get() };
    UINT strides[] = { vb->Stride, (UINT)instanceStride };
    UINT offsets[] = { 0, 0 };

    // Ставим сразу 2 буфера
    m_context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
    m_context->IASetIndexBuffer(ib->Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 3. Рисуем
    m_context->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);

    // 4. Очистка
    ID3D11ShaderResourceView* nullSRVs[8] = { nullptr };
    m_context->PSSetShaderResources(0, 8, nullSRVs);
}