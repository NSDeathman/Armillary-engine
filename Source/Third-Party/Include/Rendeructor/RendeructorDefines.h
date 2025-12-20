#pragma once

#include "RendeructorAPI.h"
#include <string>
#include <vector>
#include <map>
#include <MathAPI/MathAPI.h>

enum class ScreenMode { Windowed, Fullscreen, Borderless };
enum class RenderAPI { DirectX11, DirectX12, OpenGL, Vulkan };
enum class TextureFormat { RGBA8, RGBA16F, R16F, R32F };

enum class CullMode {
    None,
    Front,
    Back
};

enum class BlendMode {
    Opaque,
    AlphaBlend,
    Additive
};

enum class CompareFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

enum class ShaderType {
    Vertex,
    Pixel,
    Compute,
    Geometry,
    Hull,
    Domain
};

enum class TextureType {
    Tex2D,
    Tex3D,
    TexCube,
};

struct RENDER_API BackendConfig {
    int Width = 1920;
    int Height = 1080;
    ScreenMode ScreenMode = ScreenMode::Windowed;
    RenderAPI API = RenderAPI::DirectX11;
    void* WindowHandle = nullptr;
};

struct Vertex {
    Math::float3 Position;
    Math::float3 Normal;
    Math::float3 Tangent;
    Math::float3 Bitangent;
    Math::float2 UV;

    Vertex() = default;
    Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz) : Position(x, y, z), 
                                                                                        UV(u, v), 
                                                                                        Normal(nx, ny, nz),
                                                                                        Tangent(0, 0, 0), 
                                                                                        Bitangent(0, 0, 0) {}
    Vertex(Math::float3 pos, Math::float3 tangent, Math::float3 bitangent, Math::float3 normal, Math::float2 uv) : Position(pos),
                                                                                                                    UV(uv),
                                                                                                                    Normal(normal),
                                                                                                                    Tangent(tangent),
                                                                                                                    Bitangent(bitangent) {}
};

struct RENDER_API PipelineState {
    CullMode Cull = CullMode::Back;
    BlendMode Blend = BlendMode::Opaque;
    CompareFunc DepthFunc = CompareFunc::Less;
    bool DepthWrite = true;
    bool ScissorTest = false;
};

class RENDER_API Texture {
public:
    Texture() = default;

    void Create(int width, int height, TextureFormat format, const void* data = nullptr);
    bool LoadFromDisk(const std::string& path);
    void Copy(const Texture& source);

    void* GetHandle() const { return m_backendHandle; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    TextureFormat GetFormat() const { return m_format; }

private:
    void* m_backendHandle = nullptr;
    int m_width = 0;
    int m_height = 0;
    TextureFormat m_format = TextureFormat::RGBA8;
};

class RENDER_API Texture3D {
public:
    Texture3D() = default;
    void Create(int width, int height, int depth, const void* data);
    void* GetHandle() const { return m_backendHandle; }
private:
    void* m_backendHandle = nullptr;
};

class RENDER_API TextureCube {
public:
    TextureCube() = default;

    // +X (Right), -X (Left), +Y (Top), -Y (Bottom), +Z (Front), -Z (Back)
    bool LoadFromFiles(const std::vector<std::string>& filepaths);

    void* GetHandle() const { return m_backendHandle; }

private:
    void* m_backendHandle = nullptr;
};

class RENDER_API Sampler {
public:
    void Create(const std::string& filterName = "Linear");
    void* GetHandle() const { return m_backendHandle; }
private:
    void* m_backendHandle = nullptr;
};

class RENDER_API ShaderPass {
public:
    std::string PixelShaderPath;
    std::string PixelShaderEntryPoint = "main";
    std::string VertexShaderPath;
    std::string VertexShaderEntryPoint = "main";

    void AddTexture(const std::string& name, const Texture& texture);
    void AddTexture(const std::string& name, const Texture3D& texture);
    void AddTexture(const std::string& name, const TextureCube& texture);
    void AddSampler(const std::string& name, const Sampler& sampler);

    const std::map<std::string, const Texture*>& GetTextures() const { return m_textures; }
    const std::map<std::string, const Texture3D*>& GetTextures3D() const { return m_textures3D; }
    const std::map<std::string, const TextureCube*>& GetTexturesCube() const { return m_texturesCube; }
    const std::map<std::string, const Sampler*>& GetSamplers() const { return m_samplers; }

private:
    std::map<std::string, const Texture*> m_textures;
    std::map<std::string, const Texture3D*> m_textures3D;
    std::map<std::string, const TextureCube*> m_texturesCube;
    std::map<std::string, const Sampler*> m_samplers;
};

class RENDER_API Mesh {
public:
    Mesh() = default;
    void Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    bool LoadFromOBJ(const std::string& filepath);

    static void GenerateCube(Mesh& outMesh, float size = 1.0f);
    static void GeneratePlane(Mesh& outMesh, float width = 10.0f, float depth = 10.0f);
    static void GenerateScreenQuad(Mesh& outMesh);
    static void GenerateSphere(Mesh& outMesh, float radius = 1.0f, int segments = 32, int rings = 16);
    static void GenerateHemisphere(Mesh& outMesh, float radius = 1.0f, int segments = 32, int rings = 16, bool flatBottom = true);
    static void GenerateDisc(Mesh& outMesh, float radius = 1.0f, int segments = 32);
    static void GenerateTriangle(Mesh& outMesh, float size = 1.0f);

    void* GetVB() const { return m_vbHandle; }
    void* GetIB() const { return m_ibHandle; }
    int GetIndexCount() const { return m_indexCount; }

private:
    void* m_vbHandle = nullptr;
    void* m_ibHandle = nullptr;
    int m_indexCount = 0;
};

class RENDER_API InstanceBuffer {
public:
    InstanceBuffer() = default;

    void Create(const void* data, int count, int stride);

    void* GetHandle() const { return m_backendHandle; }
    int GetCount() const { return m_count; }
    int GetStride() const { return m_stride; }

private:
    void* m_backendHandle = nullptr;
    int m_count = 0;
    int m_stride = 0;
};
