#include "pch.h"
#include "Rendeructor.h"
#include "BackendDX11.h"

Rendeructor* Rendeructor::s_instance = nullptr;

Rendeructor::Rendeructor() {
    s_instance = this;
}

Rendeructor::~Rendeructor() {
    Destroy();
    if (s_instance == this) s_instance = nullptr;
}

Rendeructor* Rendeructor::GetCurrent() {
    return s_instance;
}

bool Rendeructor::Create(const BackendConfig& config) {
    m_currentConfig = config;

    if (config.API == RenderAPI::DirectX11) {
        m_backend = new BackendDX11();
    }

    if (!m_backend) return false;

    return m_backend->Initialize(config);
}

void Rendeructor::Destroy() {
    if (m_backend) {
        m_backend->Shutdown();
        delete m_backend;
        m_backend = nullptr;
    }
}

void Rendeructor::Restart(const BackendConfig& config) {
    Destroy();
    Create(config);
}

void Rendeructor::SetPipelineState(const PipelineState& state) {
    m_currentState = state;
    if (m_backend) {
        m_backend->SetPipelineState(state);
    }
}

void Rendeructor::SetCullMode(CullMode mode) {
    if (m_currentState.Cull != mode) {
        m_currentState.Cull = mode;
        // —разу пушим изменени€. 
        // ¬ будущем можно делать это лениво перед Draw call, но пока оставим сразу.
        if (m_backend) m_backend->SetPipelineState(m_currentState);
    }
}

void Rendeructor::SetBlendMode(BlendMode mode) {
    if (m_currentState.Blend != mode) {
        m_currentState.Blend = mode;
        if (m_backend) m_backend->SetPipelineState(m_currentState);
    }
}

void Rendeructor::SetDepthState(CompareFunc func, bool writeEnabled) {
    if (m_currentState.DepthFunc != func || m_currentState.DepthWrite != writeEnabled) {
        m_currentState.DepthFunc = func;
        m_currentState.DepthWrite = writeEnabled;
        if (m_backend) m_backend->SetPipelineState(m_currentState);
    }
}

void Rendeructor::SetScissorEnabled(bool enabled) {
    if (m_currentState.ScissorTest != enabled) {
        m_currentState.ScissorTest = enabled;
        if (m_backend) m_backend->SetPipelineState(m_currentState);
    }
}

void Rendeructor::SetScissor(int x, int y, int width, int height) {
    if (m_backend) m_backend->SetScissorRect(x, y, width, height);
}

void Rendeructor::SetShaderPass(ShaderPass& pass) {
    if (m_backend) {
        m_backend->PrepareShaderPass(pass);
        m_backend->SetShaderPass(pass);
    }
}

void Rendeructor::CompilePass(ShaderPass& pass) {
    if (m_backend) m_backend->PrepareShaderPass(pass);
}

void Rendeructor::SetCustomConstant(const std::string& bufferName, const void* data, size_t size) {
    if (m_backend) m_backend->UpdateConstantRaw(bufferName, data, size);
}

void Rendeructor::SetRenderTarget(const Texture& target1, const Texture& target2,
    const Texture& target3, const Texture& target4) {
    if (m_backend) {
        m_backend->SetRenderTarget(
            target1.GetHandle(),
            target2.GetHandle(),
            target3.GetHandle(),
            target4.GetHandle()
        );
    }
}

void Rendeructor::RenderPassToTexture(const Texture& target) {
    if (m_backend) {
        m_backend->SetRenderTarget(target.GetHandle());
        m_backend->DrawFullScreenQuad();
    }
}

void Rendeructor::RenderPassToScreen() {
    if (m_backend) {
        m_backend->SetRenderTarget(nullptr, nullptr, nullptr, nullptr);
        m_backend->DrawFullScreenQuad();
    }
}

void Rendeructor::Clear(float r, float g, float b, float a) {
    if (m_backend) m_backend->Clear(r, g, b, a);
}

void Rendeructor::Clear(const Texture& target, float r, float g, float b, float a) {
    if (m_backend) m_backend->ClearTexture(target.GetHandle(), r, g, b, a);
}

void Rendeructor::Clear(const Texture& t1, const Texture& t2, float r, float g, float b, float a) {
    if (m_backend) {
        m_backend->ClearTexture(t1.GetHandle(), r, g, b, a);
        m_backend->ClearTexture(t2.GetHandle(), r, g, b, a);
    }
}

void Rendeructor::Clear(const Texture& t1, const Texture& t2, const Texture& t3, float r, float g, float b, float a) {
    if (m_backend) {
        m_backend->ClearTexture(t1.GetHandle(), r, g, b, a);
        m_backend->ClearTexture(t2.GetHandle(), r, g, b, a);
        m_backend->ClearTexture(t3.GetHandle(), r, g, b, a);
    }
}

void Rendeructor::ClearDepth(float depth, int stencil) {
    if (m_backend) m_backend->ClearDepth(depth, stencil);
}

void Rendeructor::DrawMesh(const Mesh& mesh) {
    if (m_backend) {
        m_backend->DrawMesh(mesh.GetVB(), mesh.GetIB(), mesh.GetIndexCount());
    }
}

void Rendeructor::DrawMeshInstanced(const Mesh& mesh, const InstanceBuffer& instances) {
    if (m_backend) {
        m_backend->DrawMeshInstanced(
            mesh.GetVB(),
            mesh.GetIB(),
            mesh.GetIndexCount(),
            instances.GetHandle(),
            instances.GetCount(),
            instances.GetStride()
        );
    }
}

void Rendeructor::DrawFullScreenQuad() {
    if (m_backend) m_backend->DrawFullScreenQuad();
}

void Rendeructor::Present() {
    if (m_backend) {
        m_backend->EndFrame();
    }
}
