#include "pch.h"
#include "Rendeructor.h"
#include "BackendDX11.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

void Texture::Create(int width, int height, TextureFormat format, const void* data) {
    m_width = width;
    m_height = height;
    m_format = format;
    if (Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI()) {
        m_backendHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateTextureResource(width, height, (int)format, data);
    }
}

bool Texture::LoadFromDisk(const std::string& path) {
    int w, h, channels;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);

    if (!data) {
        return false;
    }

    m_width = w;
    m_height = h;
    m_format = TextureFormat::RGBA8;

    if (Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI()) {
        m_backendHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateTextureResource(w, h, (int)m_format, data);
    }

    stbi_image_free(data);
    return (m_backendHandle != nullptr);
}

void Texture::Copy(const Texture& source) {
    if (Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI()) {
        Rendeructor::GetCurrent()->GetBackendAPI()->CopyTexture(m_backendHandle, source.GetHandle());
    }
}

void Texture3D::Create(int width, int height, int depth, const void* data) {
    if (Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI()) {
        m_backendHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateTexture3DResource(width, height, depth, 0, data);
    }
}

bool TextureCube::LoadFromFiles(const std::vector<std::string>& paths) {
    if (paths.size() != 6) {
        std::cerr << "[TextureCube] Error: Need exactly 6 file paths." << std::endl;
        return false;
    }

    // Временное хранилище данных
    std::vector<const void*> pixelData(6, nullptr);
    int width = 0, height = 0, channels = 0;
    bool success = true;

    for (int i = 0; i < 6; i++) {
        int w, h, c;
        // Загружаем всегда 4 канала (RGBA)
        unsigned char* data = stbi_load(paths[i].c_str(), &w, &h, &c, 4);

        if (!data) {
            std::cerr << "[TextureCube] Failed to load face: " << paths[i] << std::endl;
            success = false;
            break;
        }

        // Проверка: все грани должны быть одного размера
        if (i == 0) {
            width = w; height = h;
        }
        else if (w != width || h != height) {
            std::cerr << "[TextureCube] Dimension mismatch in face " << i << std::endl;
            stbi_image_free(data);
            success = false;
            break;
        }

        pixelData[i] = data;
    }

    if (success && Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI()) {
        // Передаем массив указателей
        m_backendHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateTextureCubeResource(width, height, (int)TextureFormat::RGBA8, pixelData.data());
    }

    // Чистим память STB
    for (auto ptr : pixelData) {
        if (ptr) stbi_image_free((void*)ptr);
    }

    return (m_backendHandle != nullptr);
}
