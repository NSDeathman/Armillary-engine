///////////////////////////////////////////////////////////////
// Created: 21.01.2025
// Author: NS_Deathman
// Static Model Resource Definition
///////////////////////////////////////////////////////////////
#pragma once
#include "CoreAPI.h"
#include <string>
#include <memory>
#include <Rendeructor/RendeructorDefines.h> // Для классов Mesh и Texture

namespace Core::World
{
// Класс-ресурс, хранящий "тяжелые" данные геометрии и материала.
// Объекты (StaticObject) лишь ссылаются на него.
struct StaticModel
{
	std::string Name;

	// Данные геометрии (вершины, индексы)
	// Используем shared_ptr, чтобы несколько объектов могли ссылаться на один меш
	std::shared_ptr<Mesh> MeshData;

	// Основная текстура
	std::shared_ptr<Texture> Albedo;

	// Конструктор инициализирует пустые смарт-пойнтеры
	StaticModel();

	// Деструктор по умолчанию
	~StaticModel() = default;
};
} // namespace Core::World
