///////////////////////////////////////////////////////////////
// Created: 24.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <vector>
#include "render_backend_interface.h"
///////////////////////////////////////////////////////////////
namespace Render
{
class CORE_API RenderBackendDX9 : public IRenderBackend
{
  public:
	RenderBackendDX9();
	virtual ~RenderBackendDX9();

	// === IRenderBackend Implementation ===
	bool Initialize(void* windowHandle, uint32_t width, uint32_t height) override;
	void Shutdown() override;
	void Resize(uint32_t width, uint32_t height) override;

	void BeginFrame() override;
	void EndFrame() override;
	void Present() override;

	BufferHandle CreateBuffer(const BufferDesc& desc, const void* initialData = nullptr) override;
	//TextureHandle CreateTexture(const TextureDesc& desc, const void* initialData = nullptr) override;
	ShaderHandle CreateVertexShader(const void* bytecode, size_t bytecodeSize) override;
	ShaderHandle CreatePixelShader(const void* bytecode, size_t bytecodeSize) override;

	void DestroyBuffer(BufferHandle handle) override;
	void DestroyTexture(TextureHandle handle) override;
	void DestroyShader(ShaderHandle handle) override;

	void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	void SetVertexBuffer(BufferHandle handle, uint32_t stride, uint32_t offset = 0) override;
	void SetIndexBuffer(BufferHandle handle) override;
	void SetVertexShader(ShaderHandle handle) override;
	void SetPixelShader(ShaderHandle handle) override;

	void Clear(float r, float g, float b, float a, float depth = 1.0f) override;
	void Draw(uint32_t vertexCount, uint32_t startVertex = 0) override;
	void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0, uint32_t baseVertex = 0) override;

	void* GetDevice() override
	{
		return m_device;
	}

  private:
	// === Внутренние структуры для хранения ресурсов ===
	struct DX9Buffer
	{
		IDirect3DVertexBuffer9* vertexBuffer = nullptr;
		IDirect3DIndexBuffer9* indexBuffer = nullptr;
		BufferDesc desc;
		bool isMapped = false;		// Отслеживаем состояние маппинга
		void* mappedData = nullptr; // Указатель на маппедные данные

		// Статистика использования
		uint32_t mapCount = 0;
		uint32_t bindCount = 0;

		~DX9Buffer()
		{
			// Автоматическое освобождение
			if (vertexBuffer)
				vertexBuffer->Release();
			if (indexBuffer)
				indexBuffer->Release();
		}

		// Проверка валидности
		bool IsValid() const
		{
			switch (desc.type)
			{
			case BufferType::VertexBuffer:
				return vertexBuffer != nullptr;
			case BufferType::IndexBuffer:
				return indexBuffer != nullptr;
			default:
				return false;
			}
		}
	};

	struct DX9Texture
	{
		IDirect3DTexture9* texture = nullptr;
		//TextureDesc desc;
	};

	struct DX9Shader
	{
		IDirect3DVertexShader9* vertexShader = nullptr;
		IDirect3DPixelShader9* pixelShader = nullptr;
		bool isVertexShader = false;
	};

	// === Приватные методы ===
	bool CreateDevice(HWND windowHandle, uint32_t width, uint32_t height);
	void ReleaseResources();

	BufferHandle CreateVertexBufferInternal(const BufferDesc& desc, const void* data);
	BufferHandle CreateIndexBufferInternal(const BufferDesc& desc, const void* data);
	BufferHandle CreateConstantBufferInternal(const BufferDesc& desc, const void* data);

	DX9Buffer* GetBuffer(BufferHandle handle);
	DX9Texture* GetTexture(TextureHandle handle);
	DX9Shader* GetShader(ShaderHandle handle);

	//void* MapBuffer(BufferHandle handle, MapType mapType);
	//void UnmapBuffer(BufferHandle handle);
	//bool UpdateBuffer(BufferHandle handle, const void* data, uint32_t size, uint32_t offset);

	// === Члены класса ===
	IDirect3D9* m_d3d = nullptr;
	IDirect3DDevice9* m_device = nullptr;
	D3DPRESENT_PARAMETERS m_presentParams = {};

	// Менеджмент ресурсов
	std::vector<DX9Buffer> m_buffers;
	std::vector<DX9Texture> m_textures;
	std::vector<DX9Shader> m_shaders;

	uint32_t m_nextBufferId = 1;
	uint32_t m_nextTextureId = 1;
	uint32_t m_nextShaderId = 1;

	// Текущие состояния
	BufferHandle m_currentVertexBuffer = {0};
	BufferHandle m_currentIndexBuffer = {0};
	ShaderHandle m_currentVertexShader = {0};
	ShaderHandle m_currentPixelShader = {0};
};
} // namespace Render
///////////////////////////////////////////////////////////////
