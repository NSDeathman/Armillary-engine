#pragma once
#include "RendeructorDefines.h"

class BackendInterface
{
public:
    virtual ~BackendInterface() = default;

    virtual bool Initialize(const BackendConfig& config) = 0;
    virtual void Shutdown() = 0;
    virtual void Resize(int width, int height) = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;

    virtual void* GetDevice() = 0;
	virtual void* GetContext() = 0;

    // State Management
    virtual void SetPipelineState(const PipelineState& state) = 0;
    virtual void SetScissorRect(int x, int y, int width, int height) = 0;

    // Resources
    virtual void* CreateTextureResource(int width, int height, int format, const void* initialData) = 0;
    virtual void* CreateSamplerResource(const std::string& filterMode) = 0;
    virtual void* CreateTexture3DResource(int width, int height, int depth, int format, const void* initialData) = 0;
    virtual void* CreateTextureCubeResource(int width, int height, int format, const void** initialData) = 0;
    virtual void* CreateVertexBuffer(const void* data, size_t size, int stride) = 0;
    virtual void* CreateIndexBuffer(const void* data, size_t size) = 0;
    virtual void* CreateInstanceBuffer(const void* data, size_t size, int stride) = 0;

    // Operations
    virtual void CopyTexture(void* dstHandle, void* srcHandle) = 0;
    virtual void SetRenderTarget(void* target1, void* target2 = nullptr, void* target3 = nullptr, void* target4 = nullptr) = 0;
    virtual void Clear(float r, float g, float b, float a) = 0;
    virtual void ClearTexture(void* textureHandle, float r, float g, float b, float a) = 0;
    virtual void ClearDepth(float depth, int stencil) = 0;

    virtual void PrepareShaderPass(const ShaderPass& pass) = 0;
    virtual void SetShaderPass(const ShaderPass& pass) = 0;
    virtual void UpdateConstantRaw(const std::string& name, const void* data, size_t size) = 0;

    virtual void DrawFullScreenQuad() = 0;
    virtual void DrawMesh(void* vbHandle, void* ibHandle, int indexCount) = 0;
    virtual void DrawMeshInstanced(void* vbHandle, void* ibHandle, int indexCount, void* instHandle, int instanceCount, int instanceStride) = 0;
};
