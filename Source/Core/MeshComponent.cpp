#include "stdafx.h"
#include "MeshComponent.h"
#include <iostream>

namespace Core::Components
{
bool MeshComponent::LoadFromFile(const std::string& filepath)
{
	m_FilePath = filepath;

	// 1. Создаем новый ресурс
	auto newMesh = std::make_shared<Mesh>();

	// 2. Пытаемся загрузить
	if (newMesh->LoadFromOBJ(filepath))
	{
		m_Mesh = newMesh;

		// 3. АВТОМАТИЧЕСКОЕ ОБНОВЛЕНИЕ AABB
		// Берем данные, которые Mesh рассчитал при парсинге файла
		m_LocalAABB.Min = newMesh->GetBoundsMin();
		m_LocalAABB.Max = newMesh->GetBoundsMax();

		// Логирование для отладки (можно убрать)
		std::cout << "[MeshComponent] Loaded: " << filepath
		           << " AABB: " << m_LocalAABB.Min.to_string()
		           << " -> " << m_LocalAABB.Max.to_string() << std::endl;

		return true;
	}

	std::cerr << "[MeshComponent] Error: Failed to load mesh from " << filepath << std::endl;
	return false;
}

void MeshComponent::SetMesh(std::shared_ptr<Mesh> mesh)
{
	m_Mesh = mesh;

	if (m_Mesh)
	{
		m_FilePath = "Procedural/Manual"; // Маркер, что это не загружено из файла напрямую этим компонентом

		// Берем AABB из меша (работает и для процедурных кубов/сфер, если их генераторы обновлены)
		m_LocalAABB.Min = m_Mesh->GetBoundsMin();
		m_LocalAABB.Max = m_Mesh->GetBoundsMax();
	}
	else
	{
		// Сброс AABB если меш убрали
		m_LocalAABB = AABB();
	}
}

std::unique_ptr<Core::ECS::IComponent> MeshComponent::Clone() const
{
	auto clone = std::make_unique<MeshComponent>();

	// Поверхностное копирование (ссылаемся на тот же тяжелый ресурс в памяти)
	clone->m_Mesh = m_Mesh;
	clone->m_FilePath = m_FilePath;
	clone->m_LocalAABB = m_LocalAABB;

	return clone;
}
} // namespace Core::Components
