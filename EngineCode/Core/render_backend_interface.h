///////////////////////////////////////////////////////////////
// Created: 24.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "render_backend_buffer_types.h"
///////////////////////////////////////////////////////////////
namespace Render
{
	class CORE_API IRenderBackend
	{
	  public:
		virtual ~IRenderBackend() = default;

		// === Инициализация и управление ===
		virtual bool Initialize(void* windowHandle, uint32_t width, uint32_t height) = 0;
		virtual void Shutdown() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		// === Управление кадром ===
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Present() = 0;

		// === Создание ресурсов ===
		virtual BufferHandle CreateBuffer(const BufferDesc& desc, const void* initialData = nullptr) = 0;
		//virtual TextureHandle CreateTexture(const TextureDesc& desc, const void* initialData = nullptr) = 0;
		virtual ShaderHandle CreateVertexShader(const void* bytecode, size_t bytecodeSize) = 0;
		virtual ShaderHandle CreatePixelShader(const void* bytecode, size_t bytecodeSize) = 0;

		// === Уничтожение ресурсов ===
		virtual void DestroyBuffer(BufferHandle handle) = 0;
		virtual void DestroyTexture(TextureHandle handle) = 0;
		virtual void DestroyShader(ShaderHandle handle) = 0;

		// === Управление состоянием рендеринга ===
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetVertexBuffer(BufferHandle buffer, uint32_t stride, uint32_t offset = 0) = 0;
		virtual void SetIndexBuffer(BufferHandle buffer) = 0;
		virtual void SetVertexShader(ShaderHandle shader) = 0;
		virtual void SetPixelShader(ShaderHandle shader) = 0;

		// === Рендер-команды ===
		virtual void Clear(float r, float g, float b, float a, float depth = 1.0f) = 0;
		virtual void Draw(uint32_t vertexCount, uint32_t startVertex = 0) = 0;
		virtual void DrawIndexed(uint32_t indexCount, uint32_t startIndex = 0, uint32_t baseVertex = 0) = 0;

		// === Утилиты ===
		virtual void* GetDevice() = 0; // Для DX9: вернуть IDirect3DDevice9*
	};
} // namespace Render
///////////////////////////////////////////////////////////////
