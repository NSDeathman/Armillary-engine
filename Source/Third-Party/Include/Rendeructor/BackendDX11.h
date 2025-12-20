#pragma once
#include "BackendInterface.h"

using Microsoft::WRL::ComPtr;

struct DX11TextureWrapper {
    ComPtr<ID3D11Texture2D> Texture;
    ComPtr<ID3D11Texture3D> Texture3D;
    ComPtr<ID3D11ShaderResourceView> SRV;
    ComPtr<ID3D11RenderTargetView> RTV;
    int Width;
    int Height;
    int Depth;
    TextureType Type;
};

struct DX11SamplerWrapper {
    ComPtr<ID3D11SamplerState> State;
};

struct ConstantBufferVariable {
    std::string Name;
    UINT Offset;
    UINT Size;
};

struct ReflectedConstantBuffer {
    std::string Name;
    UINT Slot;
    UINT Size;
    std::vector<ConstantBufferVariable> Variables;
    ComPtr<ID3D11Buffer> HardwareBuffer;
    std::vector<uint8_t> ShadowData;
};

struct DX11ReflectionData {
    std::vector<ReflectedConstantBuffer> Buffers;
    std::map<std::string, UINT> TextureSlots;
    std::map<std::string, UINT> SamplerSlots;
};

struct DX11ShaderWrapper {
    ComPtr<ID3D11VertexShader> VertexShader;
    ComPtr<ID3D11PixelShader> PixelShader;
    ComPtr<ID3D11InputLayout> InputLayout;
    DX11ReflectionData ReflectionVS;
    DX11ReflectionData ReflectionPS;
};

struct DX11BufferWrapper {
    ComPtr<ID3D11Buffer> Buffer;
    UINT Size; // Размер в байтах
    UINT Stride; // Размер одного элемента (для VB)
};

class BackendDX11 : public BackendInterface {
public:
    BackendDX11();
    ~BackendDX11();

    bool Initialize(const BackendConfig& config) override;
    void Shutdown() override;
    void Resize(int width, int height) override;
    void BeginFrame() override;
    void EndFrame() override;

    void* GetDevice() override
	{
		return m_device.Get();
	}
	void* GetContext() override
	{
		return m_context.Get();
	}

    void SetPipelineState(const PipelineState& state) override;
    void SetScissorRect(int x, int y, int width, int height);

    void* CreateTextureResource(int width, int height, int format, const void* initialData) override;
    void* CreateTexture3DResource(int width, int height, int depth, int format, const void* initialData) override;
    void* CreateTextureCubeResource(int width, int height, int format, const void** initialData) override;
    void* CreateSamplerResource(const std::string& filterMode) override;
    void CopyTexture(void* dstHandle, void* srcHandle) override;
    void SetRenderTarget(void* target1, void* target2 = nullptr, void* target3 = nullptr, void* target4 = nullptr) override;
    void Clear(float r, float g, float b, float a) override;
    void ClearTexture(void* textureHandle, float r, float g, float b, float a) override;
    void ClearDepth(float depth, int stencil) override;
    void PrepareShaderPass(const ShaderPass& pass) override;
    void SetShaderPass(const ShaderPass& pass) override;
    void UpdateConstantRaw(const std::string& name, const void* data, size_t size) override;
    void UploadConstants(DX11ReflectionData& reflectionData, ShaderType SType);
    void DrawFullScreenQuad() override;
    void* CreateVertexBuffer(const void* data, size_t size, int stride) override;
    void* CreateIndexBuffer(const void* data, size_t size) override;
    void* CreateInstanceBuffer(const void* data, size_t size, int stride) override;
    void DrawMeshInstanced(void* vbHandle, void* ibHandle, int indexCount, void* instHandle, int instanceCount, int instanceStride) override;
    void DrawMesh(void* vbHandle, void* ibHandle, int indexCount) override;

private:
    bool InitD3D(const BackendConfig& config);
    void InitQuadGeometry();
    bool CompileShader(const std::string& path, const std::string& entry, const std::string& profile, ID3DBlob** outBlob);
    DX11ReflectionData ReflectShader(ID3DBlob* blob);
    void* CreateBufferInternal(const void* data, size_t size, UINT bindFlags);
    void CreateDepthResources(int width, int height);
    void CreateInputLayoutFromShader(const std::vector<char>& shaderBytecode, ID3D11InputLayout** outLayout);
    void SetRenderTargetsInternal(ID3D11RenderTargetView* rtvs[], int count);
    void ClearRTV(ID3D11RenderTargetView* rtv, float r, float g, float b, float a);
    void UnbindResources();
    void InitRenderStates();
    ID3D11DepthStencilState* GetDepthState(CompareFunc func, bool write);

    int m_screenWidth = 0;
    int m_screenHeight = 0;
    HWND m_hwnd = nullptr;

    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
    ComPtr<ID3D11Buffer> m_quadVertexBuffer;
    ComPtr<ID3D11Buffer> m_quadIndexBuffer;

    std::vector<DX11TextureWrapper*> m_textures;
    std::vector<DX11SamplerWrapper*> m_samplers;
    std::map<std::string, DX11ShaderWrapper> m_shaderCache;
    DX11ShaderWrapper* m_activeShader = nullptr;

    struct StoredConstant { std::vector<uint8_t> Data; };
    std::map<std::string, StoredConstant> m_cpuConstantsStorage;
    ComPtr<ID3D11Buffer> m_cbVS;
    ComPtr<ID3D11Buffer> m_cbPS;
    size_t m_cbVSSize = 0;
    size_t m_cbPSSize = 0;

    ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;

    PipelineState m_activeState;
    bool m_firstStateSet = true;

    ComPtr<ID3D11DepthStencilState> m_dssDefault;
    ComPtr<ID3D11DepthStencilState> m_dssNoWrite;

    ID3D11RenderTargetView* m_currentRTV = nullptr;
    ID3D11DepthStencilView* m_currentDSV = nullptr;

    std::vector<ID3D11RenderTargetView*> m_boundRTVs;

    CullMode m_cachedCullMode = CullMode::Back; // Допустим дефолт
    BlendMode m_cachedBlendMode = BlendMode::Opaque;
    CompareFunc m_cachedDepthFunc = CompareFunc::Less;
    bool m_cachedDepthWrite = true;
    bool m_cachedScissorEnabled = false;

    ComPtr<ID3D11RasterizerState> m_rasterizerStates[3][2];// Индекс 0: None, 1: Front, 2: Back // Умножаем на 2: набор для Scissor Disabled и Scissor Enabled

    ComPtr<ID3D11BlendState> m_blendStates[3]; // Opaque, Alpha, Additive

    std::map<uint32_t, ComPtr<ID3D11DepthStencilState>> m_depthStates;

    struct DepthBufferCacheItem {
        ComPtr<ID3D11Texture2D> Texture;
        ComPtr<ID3D11DepthStencilView> DSV;
    };

    // map автоматически сортирует ключи, для редких переключений разрешений это окей
    std::map<uint64_t, DepthBufferCacheItem> m_depthCache;

    // Хелпер: найти или создать DSV нужного размера
    ID3D11DepthStencilView* GetDepthStencilForSize(int width, int height);
};
