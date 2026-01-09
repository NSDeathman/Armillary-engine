#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <type_traits>
#include <unordered_map>
#include <bitset>
#include <cassert>
#include <utility>

namespace Core::ECS
{
class Entity;
class ComponentManager;

// =========================================================
// 0. КОНСТАНТЫ И ТИПЫ
// =========================================================
constexpr size_t MAX_COMPONENTS = 64; // Можно увеличить при необходимости

using ComponentTypeID = size_t;
using ComponentMask = std::bitset<MAX_COMPONENTS>;

// =========================================================
// 1. БАЗОВЫЙ КЛАСС КОМПОНЕНТА
// =========================================================
class IComponent
{
	friend class ComponentManager;
	friend class Entity;

  public:
	virtual ~IComponent() = default;

	// Жизненный цикл
	virtual void OnCreate()
	{
	}
	virtual void OnUpdate(float dt)
	{
	}
	virtual void OnDestroy()
	{
	}
	virtual void OnEnable()
	{
	}
	virtual void OnDisable()
	{
	}
	virtual std::unique_ptr<IComponent> Clone() const = 0;

	// Геттеры
	Entity* GetOwner() const
	{
		return m_Owner;
	}
	bool IsActive() const
	{
		return m_Active && (m_Owner ? m_Owner->IsActive() : true);
	}
	void SetActive(bool active)
	{
		if (m_Active != active)
		{
			m_Active = active;
			if (m_Active)
				OnEnable();
			else
				OnDisable();
		}
	}

  protected:
	Entity* m_Owner = nullptr;
	bool m_Active = true;

  private:
	// Только для внутреннего использования
	void SetOwner(Entity* owner)
	{
		m_Owner = owner;
	}
};

// =========================================================
// 2. СИСТЕМА ИДЕНТИФИКАЦИИ ТИПОВ (улучшенная)
// =========================================================
class ComponentTypeInfo
{
  private:
	static inline ComponentTypeID s_NextID = 0;

  public:
	template <typename T> static ComponentTypeID GetID() noexcept
	{
		static_assert(std::is_base_of_v<IComponent, T>, "T must inherit from IComponent");

		static const ComponentTypeID typeID = s_NextID++;
		assert(typeID < MAX_COMPONENTS && "Too many component types!");
		return typeID;
	}

	template <typename T> static const char* GetName() noexcept
	{
		static const char* name = typeid(T).name();
		return name;
	}
};

// =========================================================
// 3. КОМПОНЕНТНЫЙ МЕНЕДЖЕР (для оптимизации)
// =========================================================
class ComponentManager
{
  private:
	struct ComponentPool
	{
		std::vector<std::unique_ptr<IComponent>> components;
		std::vector<size_t> freeIndices;

		template <typename T, typename... Args> T* Create(Args&&... args)
		{
			if (!freeIndices.empty())
			{
				size_t index = freeIndices.back();
				freeIndices.pop_back();
				auto& component = components[index];
				component = std::make_unique<T>(std::forward<Args>(args)...);
				return static_cast<T*>(component.get());
			}
			else
			{
				components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
				return static_cast<T*>(components.back().get());
			}
		}

		void Destroy(size_t index)
		{
			components[index].reset();
			freeIndices.push_back(index);
		}
	};

	std::vector<std::unique_ptr<ComponentPool>> m_Pools;

  public:
	template <typename T, typename... Args> T* CreateComponent(Args&&... args)
	{
		ComponentTypeID typeID = ComponentTypeInfo::GetID<T>();

		if (typeID >= m_Pools.size())
			m_Pools.resize(typeID + 1);

		if (!m_Pools[typeID])
			m_Pools[typeID] = std::make_unique<ComponentPool>();

		return m_Pools[typeID]->Create<T>(std::forward<Args>(args)...);
	}

	void DestroyComponent(ComponentTypeID typeID, size_t index)
	{
		if (typeID < m_Pools.size() && m_Pools[typeID])
			m_Pools[typeID]->Destroy(index);
	}
};

// =========================================================
// 4. ДАННЫЕ СУЩНОСТИ (расширенная)
// =========================================================
struct EntityData
{
	std::string Name = "New Entity";
	std::string Tag = "Default";
	bool IsActive = true;
	bool IsStatic = false;
	bool IsPersistent = true; // Сохраняется между сценами
	uint32_t Layer = 0;		  // Слой для рендеринга/физики

	EntityData() = default;

	EntityData(std::string name, std::string tag = "Default") : Name(std::move(name)), Tag(std::move(tag))
	{
	}
};

// =========================================================
// 5. СУЩНОСТЬ (оптимизированная и удобная)
// =========================================================
class Entity
{
  private:
	// Данные сущности
	std::string m_Name;
	std::string m_Tag;
	bool m_IsActive = true;
	bool m_IsStatic = false;
	bool m_IsPersistent = true;
	uint32_t m_Layer = 0;
	uint64_t m_ID = 0; // Уникальный ID сущности

	// Компоненты
	std::vector<std::unique_ptr<IComponent>> m_Components;
	std::vector<size_t> m_ComponentIndices; // Индексы в ComponentManager
	std::unordered_map<ComponentTypeID, IComponent*> m_ComponentMap;
	ComponentMask m_ComponentMask;

	static inline ComponentManager s_ComponentManager;
	static inline uint64_t s_NextEntityID = 1;

  public:
	// Конструкторы
	Entity() : m_ID(s_NextEntityID++)
	{
	}

	explicit Entity(const EntityData& data)
		: m_Name(data.Name), m_Tag(data.Tag), m_IsActive(data.IsActive), m_IsStatic(data.IsStatic),
		  m_IsPersistent(data.IsPersistent), m_Layer(data.Layer), m_ID(s_NextEntityID++)
	{
	}

	Entity(std::string name, std::string tag = "Default")
		: m_Name(std::move(name)), m_Tag(std::move(tag)), m_ID(s_NextEntityID++)
	{
	}

	virtual ~Entity()
	{
		Destroy();
	}

	// Запрещаем копирование, разрешаем перемещение
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	Entity(Entity&& other) noexcept
		: m_Name(std::move(other.m_Name)), m_Tag(std::move(other.m_Tag)), m_IsActive(other.m_IsActive),
		  m_IsStatic(other.m_IsStatic), m_IsPersistent(other.m_IsPersistent), m_Layer(other.m_Layer), m_ID(other.m_ID),
		  m_Components(std::move(other.m_Components)), m_ComponentIndices(std::move(other.m_ComponentIndices)),
		  m_ComponentMap(std::move(other.m_ComponentMap)), m_ComponentMask(other.m_ComponentMask)
	{
		// Обновляем владельца у всех компонентов
		for (auto& component : m_Components)
			if (component)
				component->m_Owner = this;
	}

	Entity& operator=(Entity&& other) noexcept
	{
		if (this != &other)
		{
			Destroy();

			m_Name = std::move(other.m_Name);
			m_Tag = std::move(other.m_Tag);
			m_IsActive = other.m_IsActive;
			m_IsStatic = other.m_IsStatic;
			m_IsPersistent = other.m_IsPersistent;
			m_Layer = other.m_Layer;
			m_ID = other.m_ID;
			m_Components = std::move(other.m_Components);
			m_ComponentIndices = std::move(other.m_ComponentIndices);
			m_ComponentMap = std::move(other.m_ComponentMap);
			m_ComponentMask = other.m_ComponentMask;

			for (auto& component : m_Components)
				if (component)
					component->m_Owner = this;
		}
		return *this;
	}

	// Основные методы
	void Update(float deltaTime)
	{
		if (!m_IsActive)
			return;

		for (auto& component : m_Components)
			if (component && component->IsActive())
				component->OnUpdate(deltaTime);
	}

	void Destroy()
	{
		for (size_t i = 0; i < m_Components.size(); ++i)
		{
			if (m_Components[i])
			{
				m_Components[i]->OnDestroy();
				ComponentTypeID typeID =
					ComponentTypeInfo::GetID<std::remove_reference_t<decltype(*m_Components[i])>>();
				s_ComponentManager.DestroyComponent(typeID, m_ComponentIndices[i]);
			}
		}

		m_Components.clear();
		m_ComponentIndices.clear();
		m_ComponentMap.clear();
		m_ComponentMask.reset();
	}

	// =====================================================
	// УДОБНЫЙ ИНТЕРФЕЙС ДЛЯ РАБОТЫ С КОМПОНЕНТАМИ
	// =====================================================

	// 1. Добавление компонента (Fluent Interface)
	template <typename T, typename... Args> Entity& Add(Args&&... args)
	{
		AddComponent<T>(std::forward<Args>(args)...);
		return *this;
	}

	// 2. Получение компонента
	template <typename T> T* Get() const
	{
		auto it = m_ComponentMap.find(ComponentTypeInfo::GetID<T>());
		return it != m_ComponentMap.end() ? static_cast<T*>(it->second) : nullptr;
	}

	// 3. Проверка наличия компонента
	template <typename T> bool Has() const
	{
		return m_ComponentMask[ComponentTypeInfo::GetID<T>()];
	}

	// 4. Удаление компонента
	template <typename T> void Remove()
	{
		const ComponentTypeID typeID = ComponentTypeInfo::GetID<T>();
		auto it = m_ComponentMap.find(typeID);
		if (it != m_ComponentMap.end())
		{
			// Находим компонент в векторе
			for (size_t i = 0; i < m_Components.size(); ++i)
			{
				if (m_Components[i].get() == it->second)
				{
					m_Components[i]->OnDestroy();
					s_ComponentManager.DestroyComponent(typeID, m_ComponentIndices[i]);

					// Удаляем из векторов
					m_Components.erase(m_Components.begin() + i);
					m_ComponentIndices.erase(m_ComponentIndices.begin() + i);
					break;
				}
			}

			m_ComponentMap.erase(it);
			m_ComponentMask.reset(typeID);
		}
	}

	// 5. Получение или добавление компонента
	template <typename T, typename... Args> T* GetOrAdd(Args&&... args)
	{
		if (T* component = Get<T>())
			return component;
		return AddComponent<T>(std::forward<Args>(args)...);
	}

	// 6. Проверка наличия нескольких компонентов
	template <typename... Components> bool HasAll() const
	{
		return (Has<Components>() && ...);
	}

	template <typename... Components> bool HasAny() const
	{
		return (Has<Components>() || ...);
	}

	// 7. Получение всех компонентов определенного типа
	template <typename T> std::vector<T*> GetAll() const
	{
		std::vector<T*> result;
		for (auto& component : m_Components)
		{
			if (auto casted = dynamic_cast<T*>(component.get()))
				result.push_back(casted);
		}
		return result;
	}

	// Геттеры/сеттеры
	uint64_t GetID() const
	{
		return m_ID;
	}
	const std::string& GetName() const
	{
		return m_Name;
	}
	const std::string& GetTag() const
	{
		return m_Tag;
	}
	bool IsActive() const
	{
		return m_IsActive;
	}
	bool IsStatic() const
	{
		return m_IsStatic;
	}
	bool IsPersistent() const
	{
		return m_IsPersistent;
	}
	uint32_t GetLayer() const
	{
		return m_Layer;
	}
	const ComponentMask& GetComponentMask() const
	{
		return m_ComponentMask;
	}

	void SetName(const std::string& name)
	{
		m_Name = name;
	}
	void SetTag(const std::string& tag)
	{
		m_Tag = tag;
	}
	void SetActive(bool active)
	{
		m_IsActive = active;
	}
	void SetPersistent(bool persistent)
	{
		m_IsPersistent = persistent;
	}
	void SetLayer(uint32_t layer)
	{
		m_Layer = layer;
	}

	// Для систем
	template <typename Visitor> void ForEachComponent(Visitor&& visitor)
	{
		for (auto& component : m_Components)
			if (component && component->IsActive())
				visitor(*component);
	}

  private:
	// Внутренний метод добавления компонента
	template <typename T, typename... Args> T* AddComponent(Args&&... args)
	{
		const ComponentTypeID typeID = ComponentTypeInfo::GetID<T>();

		// Нельзя добавить компонент одного типа дважды
		if (m_ComponentMap.find(typeID) != m_ComponentMap.end())
		{
			// Можно либо вернуть существующий, либо assert
			assert(false && "Component of this type already exists!");
			return Get<T>();
		}

		// Создаем через ComponentManager
		T* component = s_ComponentManager.CreateComponent<T>(std::forward<Args>(args)...);
		component->SetOwner(this);

		// Сохраняем индекс в пуле
		size_t poolIndex = m_Components.size(); // Временный, реальный индекс в ComponentManager

		// Добавляем в векторы
		m_Components.emplace_back(component);
		m_ComponentIndices.push_back(poolIndex); // Заглушка, нужен реальный механизм
		m_ComponentMap[typeID] = component;
		m_ComponentMask.set(typeID);

		component->OnCreate();
		if (component->IsActive())
			component->OnEnable();

		return component;
	}
};

// =========================================================
// 6. ВСПОМОГАТЕЛЬНЫЕ ШАБЛОНЫ ДЛЯ БЛОЧНОГО КОНСТРУИРОВАНИЯ
// =========================================================

// Базовый шаблон для наследования компонентов
template <typename T> class Component : public IComponent
{
  public:
	static ComponentTypeID GetStaticTypeID()
	{
		return ComponentTypeInfo::GetID<T>();
	}

	std::unique_ptr<IComponent> Clone() const override
	{
		return std::make_unique<T>(static_cast<const T&>(*this));
	}
};

// Фабрика для удобного создания Entity
class EntityBuilder
{
  private:
	std::unique_ptr<Entity> m_Entity;

  public:
	EntityBuilder(std::string name = "Entity") : m_Entity(std::make_unique<Entity>(std::move(name)))
	{
	}

	EntityBuilder(EntityData data) : m_Entity(std::make_unique<Entity>(std::move(data)))
	{
	}

	// Fluent interface для настройки Entity
	EntityBuilder& SetName(const std::string& name)
	{
		m_Entity->SetName(name);
		return *this;
	}

	EntityBuilder& SetTag(const std::string& tag)
	{
		m_Entity->SetTag(tag);
		return *this;
	}

	EntityBuilder& SetLayer(uint32_t layer)
	{
		m_Entity->SetLayer(layer);
		return *this;
	}

	template <typename T, typename... Args> EntityBuilder& AddComponent(Args&&... args)
	{
		m_Entity->Add<T>(std::forward<Args>(args)...);
		return *this;
	}

	// Вариант с лямбдой для кастомизации компонента
	template <typename T, typename Func> EntityBuilder& AddComponent(Func&& configurator)
	{
		T* component = m_Entity->Add<T>();
		configurator(*component);
		return *this;
	}

	// Создание Entity
	std::unique_ptr<Entity> Build()
	{
		return std::move(m_Entity);
	}

	// Прямое получение указателя
	Entity* Get()
	{
		return m_Entity.get();
	}
};

// Утилита для быстрого создания Entity
inline std::unique_ptr<Entity> CreateEntity(const std::string& name)
{
	return EntityBuilder(name).Build();
}

template <typename... Components, typename... Args>
inline std::unique_ptr<Entity> CreateEntityWithComponents(const std::string& name, std::tuple<Args...> componentArgs...)
{
	auto builder = EntityBuilder(name);
	(builder.template AddComponent<Components>(std::get<Args>(componentArgs)...), ...);
	return builder.Build();
}
} // namespace Core::ECS
