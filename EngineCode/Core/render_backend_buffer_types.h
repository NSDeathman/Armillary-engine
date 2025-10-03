///////////////////////////////////////////////////////////////
// Created: 24.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
namespace Render
{
// Handle для буферов - легковесная ссылка на ресурс
struct CORE_API BufferHandle
{
	uint32_t id = 0;

	BufferHandle() = default;
	BufferHandle(uint32_t id) : id(id)
	{
	}

	bool IsValid() const
	{
		return id != 0;
	}
	bool operator==(const BufferHandle& other) const
	{
		return id == other.id;
	}
	bool operator!=(const BufferHandle& other) const
	{
		return id != other.id;
	}
};

struct CORE_API TextureHandle
{
	uint32_t id = 0;
	// ... аналогично BufferHandle
};

struct CORE_API ShaderHandle
{
	uint32_t id = 0;
	// ... аналогично BufferHandle
};

// Типы буферов
enum class CORE_API BufferType
{
	VertexBuffer,
	IndexBuffer,
	ConstantBuffer,
	StructuredBuffer
};

// Использование буфера
enum class CORE_API BufferUsage
{
	Static,		// Записывается один раз, читается много
	Dynamic,	// Часто обновляется CPU 
	Immutable	// Создается один раз, нельзя изменять
};

// CPU доступ к буферу
enum class CORE_API CpuAccess
{
	None,  // Только GPU доступ
	Write, // CPU может писать
	Read   // CPU может читать
};

// Описание буфера
struct CORE_API BufferDesc
{
	BufferType type = BufferType::VertexBuffer;
	BufferUsage usage = BufferUsage::Static;
	CpuAccess cpuAccess = CpuAccess::None;
	uint32_t size = 0;		   // Общий размер в байтах
	uint32_t stride = 0;	   // Размер элемента (для вершин/индексов)
	uint32_t elementCount = 0; // Количество элементов

	// Валидация описания
	bool IsValid() const
	{
		if (size == 0)
			return false;
		if (type == BufferType::VertexBuffer && stride == 0)
			return false;
		if (type == BufferType::IndexBuffer && stride == 0)
			return false;
		return true;
	}

	// Утилиты для создания common буферов
	static BufferDesc CreateVertexBuffer(uint32_t vertexCount, uint32_t vertexSize,
										 BufferUsage usage = BufferUsage::Static)
	{
		BufferDesc desc;
		desc.type = BufferType::VertexBuffer;
		desc.usage = usage;
		desc.stride = vertexSize;
		desc.elementCount = vertexCount;
		desc.size = vertexCount * vertexSize;
		return desc;
	}

	static BufferDesc CreateIndexBuffer(uint32_t indexCount, bool use32BitIndices = false,
										BufferUsage usage = BufferUsage::Static)
	{
		BufferDesc desc;
		desc.type = BufferType::IndexBuffer;
		desc.usage = usage;
		desc.stride = use32BitIndices ? 4 : 2;
		desc.elementCount = indexCount;
		desc.size = indexCount * desc.stride;
		return desc;
	}

	static BufferDesc CreateConstantBuffer(uint32_t size, BufferUsage usage = BufferUsage::Dynamic)
	{
		BufferDesc desc;
		desc.type = BufferType::ConstantBuffer;
		desc.usage = usage;
		desc.size = size;
		// Constant buffers должны быть выровнены
		desc.size = (desc.size + 15) & ~15; // Выравнивание до 16 байт
		return desc;
	}
};
} // namespace Render
///////////////////////////////////////////////////////////////
