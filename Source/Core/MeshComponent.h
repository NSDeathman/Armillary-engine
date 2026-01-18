#pragma once
#include "ECSCore.h"
#include <MathAPI/MathAPI.h>
#include <Rendeructor/Rendeructor.h>
#include <string>
#include <memory>
#include <cfloat>

namespace Core::ECS::Components
{
// Структура AABB (Axis-Aligned Bounding Box)
// В будущем можно вынести в MathAPI, но пока пусть живет рядом с геометрией
struct AABB
{
	Math::float3 Min = Math::float3(FLT_MAX);
	Math::float3 Max = Math::float3(-FLT_MAX);

	// Центр бокса
	Math::float3 GetCenter() const
	{
		return (Min + Max) * 0.5f;
	}
	// Размеры (ширина, высота, глубина)
	Math::float3 GetSize() const
	{
		return Max - Min;
	}

	// Проверка на валидность (если Min > Max, то бокс пуст/неинициализирован)
	bool IsValid() const
	{
		return Min.x <= Max.x;
	}

	// Расширение бокса точкой
	void Merge(const Math::float3& point)
	{
		Min = Math::float3::min(Min, point);
		Max = Math::float3::max(Max, point);
	}

	// Расширение бокса другим боксом
	void Merge(const AABB& other)
	{
		if (!other.IsValid())
			return;
		Min = Math::float3::min(Min, other.Min);
		Max = Math::float3::max(Max, other.Max);
	}
};

class MeshComponent : public Core::ECS::Component<MeshComponent>
{
  public:
	MeshComponent() = default;
	virtual ~MeshComponent() = default;

	// --- Загрузка и Инициализация ---

	/**
	 * @brief Загружает меш из OBJ файла и автоматически рассчитывает AABB.
	 * @param filepath Путь к файлу (относительно рабочей директории или ресурсов)
	 * @return true если загрузка удалась
	 */
	bool LoadFromFile(const std::string& filepath);

	/**
	 * @brief Устанавливает уже существующий меш (например, процедурный).
	 *        Автоматически берет AABB из данных меша.
	 * @param mesh Умный указатель на меш
	 */
	void SetMesh(std::shared_ptr<Mesh> mesh);

	// --- Геттеры ---

	std::shared_ptr<Mesh> GetMesh() const
	{
		return m_Mesh;
	}
	const std::string& GetFilePath() const
	{
		return m_FilePath;
	}

	// Возвращает локальный AABB (в координатах модели, без учета трансформации Entity)
	const AABB& GetLocalAABB() const
	{
		return m_LocalAABB;
	}

	// Метод для клонирования компонента (требование ECS)
	std::unique_ptr<Core::ECS::IComponent> Clone() const override;

  protected:
	// Ссылка на ресурс меша (Shared, так как один меш может использоваться множеством сущностей)
	std::shared_ptr<Mesh> m_Mesh;

	// Путь к файлу (пусто, если меш сгенерирован процедурно)
	std::string m_FilePath;

	// Локальные границы объекта
	AABB m_LocalAABB;
};
} // namespace Core::Components
