#pragma once
#include "RendeructorDefines.h"
#include "BackendInterface.h"

class RENDER_API Rendeructor {
public:
    Rendeructor();
    ~Rendeructor();

    bool Create(const BackendConfig& config);
    void Restart(const BackendConfig& config);
    void Destroy();

    void* GetDevice()
	{
		return m_backend ? m_backend->GetDevice() : nullptr;
	}
	void* GetContext()
	{
		return m_backend ? m_backend->GetContext() : nullptr;
	}

    void SetShaderPass(ShaderPass& pass);
    void CompilePass(ShaderPass& pass);

    PipelineState GetPipelineState() const { return m_currentState; }
    void SetPipelineState(const PipelineState& state);
    void SetCullMode(CullMode mode);
    void SetBlendMode(BlendMode mode);
    void SetDepthState(CompareFunc func, bool writeEnabled);
    void SetScissorEnabled(bool enabled);
    void SetScissor(int x, int y, int width, int height);

    template<typename T>
    void SetConstant(const std::string& name, const T& value) {
        if (m_backend) m_backend->UpdateConstantRaw(name, &value, sizeof(T));
    }
    void SetCustomConstant(const std::string& bufferName, const void* data, size_t size);
    template <typename T>
    void SetCustomConstant(const std::string& bufferName, const T& dataStructure) {
        SetCustomConstant(bufferName, &dataStructure, sizeof(T));
    }

    void SetRenderTarget(const Texture& target1 = Texture(),
                         const Texture& target2 = Texture(),
                         const Texture& target3 = Texture(),
                         const Texture& target4 = Texture());
    void RenderPassToTexture(const Texture& target);
    void RenderPassToScreen();
    void Clear(float r, float g, float b, float a = 1.0f);
    void Clear(const Texture& target, float r, float g, float b, float a = 1.0f);
    void Clear(const Texture& t1, const Texture& t2, float r, float g, float b, float a = 1.0f);
    void Clear(const Texture& t1, const Texture& t2, const Texture& t3, float r, float g, float b, float a = 1.0f);
    void ClearDepth(float depth = 1.0f, int stencil = 0);

    void DrawFullScreenQuad();
    void DrawMesh(const Mesh& mesh);
    void DrawMeshInstanced(const Mesh& mesh, const InstanceBuffer& instances);
    void Present();

    static Rendeructor* GetCurrent();
    BackendInterface* GetBackendAPI() { return m_backend; }

private:
    BackendInterface* m_backend = nullptr;
    PipelineState m_currentState;
    BackendConfig m_currentConfig;
    static Rendeructor* s_instance;
};
