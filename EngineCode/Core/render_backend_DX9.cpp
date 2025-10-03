///////////////////////////////////////////////////////////////
// Created: 24.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#include "render_backend_DX9.h"
///////////////////////////////////////////////////////////////
namespace Render
{
const D3DVERTEXELEMENT9 VERTEX_DECL[] = {{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
										 {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
										 {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
										 D3DDECL_END()};

RenderBackendDX9::RenderBackendDX9() : m_d3d(nullptr), m_device(nullptr)
{
}

RenderBackendDX9::~RenderBackendDX9()
{
	Shutdown();
}

bool RenderBackendDX9::Initialize(void* windowHandle, uint32_t width, uint32_t height)
{
	if (!windowHandle)
	{
		std::cerr << "Invalid window handle!" << std::endl;
		return false;
	}

	// Создаем Direct3D объект
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_d3d)
	{
		std::cerr << "Failed to create Direct3D9 object!" << std::endl;
		return false;
	}

	// Создаем устройство
	if (!CreateDevice(static_cast<HWND>(windowHandle), width, height))
	{
		std::cerr << "Failed to create D3D9 device!" << std::endl;
		Shutdown();
		return false;
	}

	std::cout << "RenderBackendDX9 initialized successfully!" << std::endl;
	return true;
}

void RenderBackendDX9::Shutdown()
{
	ReleaseResources();

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_d3d)
	{
		m_d3d->Release();
		m_d3d = nullptr;
	}

	std::cout << "RenderBackendDX9 shutdown complete." << std::endl;
}

bool RenderBackendDX9::CreateDevice(HWND windowHandle, uint32_t width, uint32_t height)
{
	ZeroMemory(&m_presentParams, sizeof(m_presentParams));

	m_presentParams.Windowed = TRUE;
	m_presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_presentParams.BackBufferFormat = D3DFMT_UNKNOWN;
	m_presentParams.BackBufferCount = 1;
	m_presentParams.BackBufferWidth = width;
	m_presentParams.BackBufferHeight = height;
	m_presentParams.hDeviceWindow = windowHandle;
	m_presentParams.EnableAutoDepthStencil = TRUE;
	m_presentParams.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_presentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // VSync

	// Пытаемся создать устройство с аппаратным ускорением
	HRESULT hr =
		m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, windowHandle,
							D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &m_presentParams, &m_device);

	// Fallback на софтверную обработку вершин если нужно
	if (FAILED(hr))
	{
		hr = m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, windowHandle,
								 D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &m_presentParams,
								 &m_device);
	}

	if (FAILED(hr))
	{
		std::cerr << "Failed to create D3D9 device! HRESULT: " << hr << std::endl;
		return false;
	}

	// Устанавливаем базовые состояния рендеринга
	m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_device->SetRenderState(D3DRS_ZENABLE, TRUE);
	m_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return true;
}

void RenderBackendDX9::ReleaseResources()
{
	// Уничтожаем шейдеры
	for (auto& shader : m_shaders)
	{
		if (shader.vertexShader)
			shader.vertexShader->Release();
		if (shader.pixelShader)
			shader.pixelShader->Release();
	}
	m_shaders.clear();

	// Уничтожаем текстуры
	for (auto& texture : m_textures)
	{
		if (texture.texture)
			texture.texture->Release();
	}
	m_textures.clear();

	// Уничтожаем буферы
	for (auto& buffer : m_buffers)
	{
		if (buffer.vertexBuffer)
			buffer.vertexBuffer->Release();
		if (buffer.indexBuffer)
			buffer.indexBuffer->Release();
	}
	m_buffers.clear();

	m_nextBufferId = 1;
	m_nextTextureId = 1;
	m_nextShaderId = 1;
}

void RenderBackendDX9::Resize(uint32_t width, uint32_t height)
{
	if (!m_device || width == 0 || height == 0)
		return;

	// Освобождаем ресурсы, которые зависят от размера окна
	// (в DX9 обычно это делается автоматически при потере устройства)

	m_presentParams.BackBufferWidth = width;
	m_presentParams.BackBufferHeight = height;

	// Сбрасываем устройство с новыми параметрами
	HRESULT hr = m_device->Reset(&m_presentParams);
	if (FAILED(hr))
	{
		std::cerr << "Failed to reset D3D9 device on resize!" << std::endl;
	}
}

void RenderBackendDX9::BeginFrame()
{
	if (!m_device)
		return;

	// Очищаем бэкбуфер и глубину
	Clear(0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
}

void RenderBackendDX9::EndFrame()
{
	// В DX9 обычно не нужно делать ничего специального в EndFrame
}

void RenderBackendDX9::Present()
{
	if (!m_device)
		return;

	HRESULT hr = m_device->Present(nullptr, nullptr, nullptr, nullptr);
	if (hr == D3DERR_DEVICELOST)
	{
		// Устройство потеряно - пытаемся восстановить
		hr = m_device->TestCooperativeLevel();
		if (hr == D3DERR_DEVICENOTRESET)
		{
			// Можно попытаться восстановить устройство
			std::cout << "Device lost, attempting to reset..." << std::endl;
		}
	}
}

// === Buffer Management ===
BufferHandle RenderBackendDX9::CreateBuffer(const BufferDesc& desc, const void* initialData)
{
	if (!desc.IsValid())
	{
		std::cerr << "Invalid buffer description!" << std::endl;
		return BufferHandle();
	}

	if (!m_device)
	{
		std::cerr << "Device not initialized!" << std::endl;
		return BufferHandle();
	}

	BufferHandle handle;

	switch (desc.type)
	{
	case BufferType::VertexBuffer:
		handle = CreateVertexBufferInternal(desc, initialData);
		break;

	case BufferType::IndexBuffer:
		handle = CreateIndexBufferInternal(desc, initialData);
		break;

	case BufferType::ConstantBuffer:
		// В DX9 нет отдельного типа для constant buffer - используем vertex buffer
		handle = CreateConstantBufferInternal(desc, initialData);
		break;

	default:
		std::cerr << "Unsupported buffer type!" << std::endl;
		return BufferHandle();
	}

	if (handle.IsValid())
	{
		std::cout << "Created buffer: type=" << static_cast<int>(desc.type) << ", size=" << desc.size << " bytes"
				  << std::endl;
	}

	return handle;
}

// === CreateVertexBufferInternal ===
BufferHandle RenderBackendDX9::CreateVertexBufferInternal(const BufferDesc& desc, const void* data)
{
	IDirect3DVertexBuffer9* vertexBuffer = nullptr;

	// Определяем параметры создания
	DWORD usage = D3DUSAGE_WRITEONLY;
	D3DPOOL pool = D3DPOOL_MANAGED;

	if (desc.usage == BufferUsage::Dynamic)
	{
		usage |= D3DUSAGE_DYNAMIC;
		pool = D3DPOOL_DEFAULT; // Dynamic buffers должны быть в DEFAULT pool
	}

	HRESULT hr = m_device->CreateVertexBuffer(desc.size, usage,
											  0, // FVF - 0 т.к. используем vertex declaration
											  pool, &vertexBuffer, nullptr);

	if (FAILED(hr))
	{
		std::cerr << "Failed to create vertex buffer! HRESULT: " << hr << std::endl;
		return BufferHandle();
	}

	// Создаем внутреннюю структуру
	DX9Buffer dx9Buffer;
	dx9Buffer.vertexBuffer = vertexBuffer;
	dx9Buffer.desc = desc;

	// Заполняем начальными данными если нужно
	if (data)
	{
		void* bufferData = nullptr;
		hr = vertexBuffer->Lock(0, 0, &bufferData, 0);
		if (SUCCEEDED(hr))
		{
			memcpy(bufferData, data, desc.size);
			vertexBuffer->Unlock();
		}
	}

	// Сохраняем и возвращаем handle
	BufferHandle handle = {m_nextBufferId++};
	m_buffers.push_back(std::move(dx9Buffer));

	return handle;
}

// === CreateIndexBufferInternal ===
BufferHandle RenderBackendDX9::CreateIndexBufferInternal(const BufferDesc& desc, const void* data)
{
	IDirect3DIndexBuffer9* indexBuffer = nullptr;

	DWORD usage = D3DUSAGE_WRITEONLY;
	D3DPOOL pool = D3DPOOL_MANAGED;
	D3DFORMAT format = (desc.stride == 4) ? D3DFMT_INDEX32 : D3DFMT_INDEX16;

	if (desc.usage == BufferUsage::Dynamic)
	{
		usage |= D3DUSAGE_DYNAMIC;
		pool = D3DPOOL_DEFAULT;
	}

	HRESULT hr = m_device->CreateIndexBuffer(desc.size, usage, format, pool, &indexBuffer, nullptr);

	if (FAILED(hr))
	{
		std::cerr << "Failed to create index buffer! HRESULT: " << hr << std::endl;
		return BufferHandle();
	}

	DX9Buffer dx9Buffer;
	dx9Buffer.indexBuffer = indexBuffer;
	dx9Buffer.desc = desc;

	if (data)
	{
		void* bufferData = nullptr;
		hr = indexBuffer->Lock(0, 0, &bufferData, 0);
		if (SUCCEEDED(hr))
		{
			memcpy(bufferData, data, desc.size);
			indexBuffer->Unlock();
		}
	}

	BufferHandle handle = {m_nextBufferId++};
	m_buffers.push_back(std::move(dx9Buffer));

	return handle;
}

// === CreateConstantBufferInternal ===
BufferHandle RenderBackendDX9::CreateConstantBufferInternal(const BufferDesc& desc, const void* data)
{
	// В DX9 constant buffers эмулируются через vertex buffers
	BufferDesc vertexBufferDesc = desc;
	vertexBufferDesc.type = BufferType::VertexBuffer;
	vertexBufferDesc.stride = desc.size; // Для constant buffer stride = size

	return CreateVertexBufferInternal(vertexBufferDesc, data);
}

// === Map/Unmap для динамических буферов ===
/*
void* RenderBackendDX9::MapBuffer(BufferHandle handle, MapType mapType)
{
	if (!handle.IsValid())
		return nullptr;

	DX9Buffer* buffer = GetBuffer(handle);
	if (!buffer || buffer->isMapped)
	{
		return nullptr;
	}

	DWORD lockFlags = 0;
	switch (mapType)
	{
	case MapType::Read:
		lockFlags = D3DLOCK_READONLY;
		break;
	case MapType::Write:
		if (buffer->desc.usage == BufferUsage::Dynamic)
		{
			lockFlags = D3DLOCK_DISCARD;
		}
		break;
	case MapType::ReadWrite:
		// Без специальных флагов
		break;
	}

	void* mappedData = nullptr;
	HRESULT hr = S_OK;

	switch (buffer->desc.type)
	{
	case BufferType::VertexBuffer:
		hr = buffer->vertexBuffer->Lock(0, 0, &mappedData, lockFlags);
		break;
	case BufferType::IndexBuffer:
		hr = buffer->indexBuffer->Lock(0, 0, &mappedData, lockFlags);
		break;
	default:
		return nullptr;
	}

	if (SUCCEEDED(hr))
	{
		buffer->isMapped = true;
		buffer->mappedData = mappedData;
		buffer->mapCount++;
		return mappedData;
	}

	return nullptr;
}

void RenderBackendDX9::UnmapBuffer(BufferHandle handle)
{
	if (!handle.IsValid())
		return;

	DX9Buffer* buffer = GetBuffer(handle);
	if (!buffer || !buffer->isMapped)
		return;

	HRESULT hr = S_OK;
	switch (buffer->desc.type)
	{
	case BufferType::VertexBuffer:
		hr = buffer->vertexBuffer->Unlock();
		break;
	case BufferType::IndexBuffer:
		hr = buffer->indexBuffer->Unlock();
		break;
	default:
		return;
	}

	if (SUCCEEDED(hr))
	{
		buffer->isMapped = false;
		buffer->mappedData = nullptr;
	}
}

// === UpdateBuffer (удобная обертка) ===
bool RenderBackendDX9::UpdateBuffer(BufferHandle handle, const void* data, uint32_t size, uint32_t offset)
{
	if (!handle.IsValid() || !data)
		return false;

	DX9Buffer* buffer = GetBuffer(handle);
	if (!buffer)
		return false;

	// Проверяем размер
	uint32_t actualSize = (size == 0) ? buffer->desc.size : size;
	if (offset + actualSize > buffer->desc.size)
	{
		std::cerr << "Buffer update exceeds buffer size!" << std::endl;
		return false;
	}

	// Для динамических буферов используем DISCARD
	if (buffer->desc.usage == BufferUsage::Dynamic)
	{
		void* mapped = MapBuffer(handle, MapType::Write);
		if (mapped)
		{
			memcpy(static_cast<char*>(mapped) + offset, data, actualSize);
			UnmapBuffer(handle);
			return true;
		}
	}
	else
	{
		// Для статических - обычный lock
		void* mappedData = nullptr;
		HRESULT hr = S_OK;

		switch (buffer->desc.type)
		{
		case BufferType::VertexBuffer:
			hr = buffer->vertexBuffer->Lock(offset, actualSize, &mappedData, 0);
			break;
		case BufferType::IndexBuffer:
			hr = buffer->indexBuffer->Lock(offset, actualSize, &mappedData, 0);
			break;
		default:
			return false;
		}

		if (SUCCEEDED(hr) && mappedData)
		{
			memcpy(mappedData, data, actualSize);

			switch (buffer->desc.type)
			{
			case BufferType::VertexBuffer:
				buffer->vertexBuffer->Unlock();
				break;
			case BufferType::IndexBuffer:
				buffer->indexBuffer->Unlock();
				break;
			}
			return true;
		}
	}

	return false;
}
*/
/*
TextureHandle RenderBackendDX9::CreateTexture(const TextureDesc& desc, const void* initialData)
{
	if (!m_device)
		return {0};

	IDirect3DTexture9* texture = nullptr;
	D3DFORMAT format = D3DFMT_A8R8G8B8; // Упрощенно - всегда RGBA

	HRESULT hr = m_device->CreateTexture(desc.width, desc.height, desc.mipLevels,
										 0, // Usage
										 format, D3DPOOL_MANAGED, &texture, nullptr);

	if (FAILED(hr) || !texture)
	{
		std::cerr << "Failed to create texture! HRESULT: " << hr << std::endl;
		return {0};
	}

	// Заполняем данными если нужно
	if (initialData)
	{
		D3DLOCKED_RECT lockedRect;
		hr = texture->LockRect(0, &lockedRect, nullptr, 0);
		if (SUCCEEDED(hr))
		{
			// Простая копия (для реального использования нужна конвертация форматов)
			const uint32_t* src = static_cast<const uint32_t*>(initialData);
			uint32_t* dst = static_cast<uint32_t*>(lockedRect.pBits);

			for (uint32_t y = 0; y < desc.height; ++y)
			{
				memcpy(dst, src, desc.width * 4); // 4 bytes per pixel
				src += desc.width;
				dst += lockedRect.Pitch / 4;
			}

			texture->UnlockRect(0);
		}
	}

	DX9Texture dx9Texture;
	dx9Texture.texture = texture;
	dx9Texture.desc = desc;

	TextureHandle handle = {m_nextTextureId++};
	m_textures.push_back(dx9Texture);

	return handle;
}
*/
ShaderHandle RenderBackendDX9::CreateVertexShader(const void* bytecode, size_t bytecodeSize)
{
	if (!m_device || !bytecode || bytecodeSize == 0)
		return {0};

	IDirect3DVertexShader9* vertexShader = nullptr;
	HRESULT hr = m_device->CreateVertexShader(static_cast<const DWORD*>(bytecode), &vertexShader);

	if (FAILED(hr) || !vertexShader)
	{
		std::cerr << "Failed to create vertex shader! HRESULT: " << hr << std::endl;
		return {0};
	}

	DX9Shader dx9Shader;
	dx9Shader.vertexShader = vertexShader;
	dx9Shader.isVertexShader = true;

	ShaderHandle handle = {m_nextShaderId++};
	m_shaders.push_back(dx9Shader);

	return handle;
}

ShaderHandle RenderBackendDX9::CreatePixelShader(const void* bytecode, size_t bytecodeSize)
{
	if (!m_device || !bytecode || bytecodeSize == 0)
		return {0};

	IDirect3DPixelShader9* pixelShader = nullptr;
	HRESULT hr = m_device->CreatePixelShader(static_cast<const DWORD*>(bytecode), &pixelShader);

	if (FAILED(hr) || !pixelShader)
	{
		std::cerr << "Failed to create pixel shader! HRESULT: " << hr << std::endl;
		return {0};
	}

	DX9Shader dx9Shader;
	dx9Shader.pixelShader = pixelShader;
	dx9Shader.isVertexShader = false;

	ShaderHandle handle = {m_nextShaderId++};
	m_shaders.push_back(dx9Shader);

	return handle;
}

// === Resource Destruction ===
void RenderBackendDX9::DestroyBuffer(BufferHandle handle)
{
	if (!handle.IsValid())
		return;

	// Если буфер сейчас привязан - отвязываем
	if (m_currentVertexBuffer == handle)
	{
		//m_device->SetStreamSource(0, nullptr, 0, 0);
		m_currentVertexBuffer = BufferHandle();
	}

	if (m_currentIndexBuffer == handle)
	{
		m_device->SetIndices(nullptr);
		m_currentIndexBuffer = BufferHandle();
	}

	// Находим и очищаем буфер
	DX9Buffer* buffer = GetBuffer(handle);
	if (buffer)
	{
		// Ресурсы автоматически освободятся в деструкторе DX9Buffer
		// Просто отмечаем как invalid
		buffer->vertexBuffer = nullptr;
		buffer->indexBuffer = nullptr;
	}
}


void RenderBackendDX9::DestroyTexture(TextureHandle handle)
{
	if (handle.id == 0 || handle.id >= m_nextTextureId)
		return;

	DX9Texture* texture = GetTexture(handle);
	if (texture && texture->texture)
	{
		texture->texture->Release();
		texture->texture = nullptr;
	}
}

void RenderBackendDX9::DestroyShader(ShaderHandle handle)
{
	if (handle.id == 0 || handle.id >= m_nextShaderId)
		return;

	DX9Shader* shader = GetShader(handle);
	if (shader)
	{
		if (shader->vertexShader)
			shader->vertexShader->Release();
		if (shader->pixelShader)
			shader->pixelShader->Release();
		shader->vertexShader = nullptr;
		shader->pixelShader = nullptr;
	}
}

// === Rendering Commands ===
void RenderBackendDX9::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	if (!m_device)
		return;

	D3DVIEWPORT9 viewport;
	viewport.X = x;
	viewport.Y = y;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	m_device->SetViewport(&viewport);
}

void RenderBackendDX9::SetVertexBuffer(BufferHandle handle, uint32_t stride, uint32_t offset)
{
	if (!m_device || !handle.IsValid())
	{
		return;
	}

	// Проверяем, не тот ли буфер уже установлен (оптимизация)
	if (m_currentVertexBuffer == handle)
	{
		return;
	}

	// Устанавливаем vertex declaration ПЕРЕД stream source
	static IDirect3DVertexDeclaration9* vertexDecl = nullptr;
	if (!vertexDecl)
	{
		// Создаем declaration если еще не создан
		m_device->CreateVertexDeclaration(VERTEX_DECL, &vertexDecl);
	}
	m_device->SetVertexDeclaration(vertexDecl); // ← ВАЖНО!

	DX9Buffer* buffer = GetBuffer(handle);
	if (!buffer || !buffer->vertexBuffer)
	{
		std::cerr << "Invalid vertex buffer handle!" << std::endl;
		return;
	}

	if (buffer->desc.type != BufferType::VertexBuffer)
	{
		std::cerr << "Buffer is not a vertex buffer!" << std::endl;
		return;
	}

	// Используем переданный stride или из описания буфера
	uint32_t actualStride = (stride != 0) ? stride : buffer->desc.stride;

	HRESULT hr = m_device->SetStreamSource(0, buffer->vertexBuffer, offset, actualStride);
	if (FAILED(hr))
	{
		std::cerr << "Failed to set vertex buffer! HRESULT: " << hr << std::endl;
		return;
	}

	m_currentVertexBuffer = handle;
	buffer->bindCount++; // Статистика
}

void RenderBackendDX9::SetIndexBuffer(BufferHandle handle)
{
	if (!m_device || !handle.IsValid())
	{
		return;
	}

	if (m_currentIndexBuffer == handle)
	{
		return;
	}

	DX9Buffer* buffer = GetBuffer(handle);
	if (!buffer || !buffer->indexBuffer)
	{
		std::cerr << "Invalid index buffer handle!" << std::endl;
		return;
	}

	if (buffer->desc.type != BufferType::IndexBuffer)
	{
		std::cerr << "Buffer is not an index buffer!" << std::endl;
		return;
	}

	HRESULT hr = m_device->SetIndices(buffer->indexBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to set index buffer! HRESULT: " << hr << std::endl;
		return;
	}

	m_currentIndexBuffer = handle;
	buffer->bindCount++; // Статистика
}

void RenderBackendDX9::SetVertexShader(ShaderHandle handle)
{
	if (!m_device || handle.id == 0)
		return;

	DX9Shader* shader = GetShader(handle);
	if (!shader || !shader->vertexShader)
		return;

	m_device->SetVertexShader(shader->vertexShader);
	m_currentVertexShader = handle;
}

void RenderBackendDX9::SetPixelShader(ShaderHandle handle)
{
	if (!m_device || handle.id == 0)
		return;

	DX9Shader* shader = GetShader(handle);
	if (!shader || !shader->pixelShader)
		return;

	m_device->SetPixelShader(shader->pixelShader);
	m_currentPixelShader = handle;
}

void RenderBackendDX9::Clear(float r, float g, float b, float a, float depth)
{
	if (!m_device)
		return;

	DWORD clearFlags = D3DCLEAR_TARGET;
	if (m_presentParams.EnableAutoDepthStencil)
	{
		clearFlags |= D3DCLEAR_ZBUFFER;
	}

	m_device->Clear(0, nullptr, clearFlags,
					D3DCOLOR_RGBA(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255),
								  static_cast<int>(a * 255)),
					depth, 0);
}

void RenderBackendDX9::Draw(uint32_t vertexCount, uint32_t startVertex)
{
	if (!m_device)
		return;

	// Устанавливаем vertex declaration
	static IDirect3DVertexDeclaration9* vertexDecl = nullptr;
	if (!vertexDecl)
	{
		m_device->CreateVertexDeclaration(VERTEX_DECL, &vertexDecl);
	}
	m_device->SetVertexDeclaration(vertexDecl);

	m_device->DrawPrimitive(D3DPT_TRIANGLELIST, startVertex, vertexCount / 3);
}

void RenderBackendDX9::DrawIndexed(uint32_t indexCount, uint32_t startIndex, uint32_t baseVertex)
{
	if (!m_device)
		return;

	// Устанавливаем vertex declaration
	static IDirect3DVertexDeclaration9* vertexDecl = nullptr;
	if (!vertexDecl)
	{
		m_device->CreateVertexDeclaration(VERTEX_DECL, &vertexDecl);
	}
	m_device->SetVertexDeclaration(vertexDecl);

	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, baseVertex, 0, indexCount, startIndex, indexCount / 3);
}

// === Internal Helpers ===
RenderBackendDX9::DX9Buffer* RenderBackendDX9::GetBuffer(BufferHandle handle)
{
	if (!handle.IsValid() || handle.id > m_buffers.size())
	{
		return nullptr;
	}
	return &m_buffers[handle.id - 1];
}

//const BufferDesc* RenderBackendDX9::GetBufferDesc(BufferHandle handle)
//{
//	DX9Buffer* buffer = GetBuffer(handle);
//	return buffer ? &buffer->desc : nullptr;
//}

RenderBackendDX9::DX9Texture* RenderBackendDX9::GetTexture(TextureHandle handle)
{
	if (handle.id == 0 || handle.id > m_textures.size())
		return nullptr;
	return &m_textures[handle.id - 1];
}

RenderBackendDX9::DX9Shader* RenderBackendDX9::GetShader(ShaderHandle handle)
{
	if (handle.id == 0 || handle.id > m_shaders.size())
		return nullptr;
	return &m_shaders[handle.id - 1];
}
} // namespace Render
///////////////////////////////////////////////////////////////
