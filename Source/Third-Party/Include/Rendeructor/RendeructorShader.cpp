#include "pch.h"
#include "Rendeructor.h"
#include "BackendDX11.h"

void ShaderPass::AddTexture(const std::string& name, const Texture& texture) {
    m_textures[name] = &texture;
}

void ShaderPass::AddTexture(const std::string& name, const Texture3D& texture) {
    m_textures3D[name] = &texture;
}

void ShaderPass::AddTexture(const std::string& name, const TextureCube& texture) {
    m_texturesCube[name] = &texture;
}

void ShaderPass::AddSampler(const std::string& name, const Sampler& sampler) {
    m_samplers[name] = &sampler;
}

void Sampler::Create(const std::string& filterName) {
    if (Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI()) {
        m_backendHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateSamplerResource(filterName);
    }
}