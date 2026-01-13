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
#include <tuple>

namespace Core::ECS
{
class Entity;
class ComponentManager;

// =========================================================
// 0. КОНСТАНТЫ И ТИПЫ
// =========================================================
constexpr size_t MAX_COMPONENTS = 64;

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

	// Геттеры (Реализация внизу файла, чтобы избежать circular dependency)
	inline Entity* GetOwner() const;
	inline bool IsActive() const;
	inline void SetActive(bool active);

  protected:
	Entity* m_Owner = nullptr;
	bool m_Active = true;

  private:
	void SetOwner(Entity* owner)
	{
		m_Owner = owner;
	}
};

// =========================================================
// 2. СИСТЕМА ИДЕНТИФИКАЦИИ ТИПОВ
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
		return typeid(T).name();
	}
};

// =========================================================
// 3. КОМПОНЕНТНЫЙ МЕНЕДЖЕР
// =========================================================
class ComponentManager
{
  public:
	// Структура результата создания, чтобы Entity знала реальный индекс
	template <typename T> struct CreateResult
	{
		T* Ptr;
		size_t PoolIndex;
	};

  private:
	struct ComponentPool
	{
		std::vector<std::unique_ptr<IComponent>> components;
		std::vector<size_t> freeIndices;

		// Возвращает пару: {Указатель, Индекс}
		template <typename T, typename... Args> CreateResult<T> Create(Args&&... args)
		{
			size_t index;
			if (!freeIndices.empty())
			{
				index = freeIndices.back();
				freeIndices.pop_back();
				components[index] = std::make_unique<T>(std::forward<Args>(args)...);
			}
			else
			{
				index = components.size();
				components.push_back(std::make_unique<T>(std::forward<Args>(args)...));
			}

			return {static_cast<T*>(components[index].get()), index};
		}

		void Destroy(size_t index)
		{
			if (index < components.size())
			{
				components[index].reset(); // Освобождаем память
				freeIndices.push_back(index);
			}
		}
	};

	std::vector<std::unique_ptr<ComponentPool>> m_Pools;

  public:
	template <typename T, typename... Args> CreateResult<T> CreateComponent(Args&&... args)
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

	void Clear()
	{
		m_Pools.clear();
		m_Pools.shrink_to_fit();
	}
};

// =========================================================
// 4. ДАННЫЕ СУЩНОСТИ
// =========================================================
struct EntityData
{
	std::string Name = "New Entity";
	std::string Tag = "Default";
	bool IsActive = true;
	bool IsStatic = false;
	bool IsPersistent = true;
	uint32_t Layer = 0;

	EntityData() = default;
	EntityData(std::string name, std::string tag = "Default") : Name(std::move(name)), Tag(std::move(tag))
	{
	}
};

// =========================================================
// 5. СУЩНОСТЬ
// =========================================================
class Entity
{
  private:
	// Данные
	std::string m_Name;
	std::string m_Tag;
	bool m_IsActive = true;
	bool m_IsStatic = false;
	bool m_IsPersistent = true;
	uint32_t m_Layer = 0;
	uint64_t m_ID = 0;

	// Компоненты
	// FIX: Используем сырые указатели, так как владелец - ComponentManager
	std::vector<IComponent*> m_Components;

	// Индексы в пулах ComponentManager'а
	std::vector<size_t> m_ComponentIndices;

	std::unordered_map<ComponentTypeID, IComponent*> m_ComponentMap;
	ComponentMask m_ComponentMask;

	static inline ComponentManager s_ComponentManager;
	static inline uint64_t s_NextEntityID = 1;

  public:
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

	// Запрещаем копирование
	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	// Конструктор перемещения
	Entity(Entity&& other) noexcept
		: m_Name(std::move(other.m_Name)), m_Tag(std::move(other.m_Tag)), m_IsActive(other.m_IsActive),
		  m_IsStatic(other.m_IsStatic), m_IsPersistent(other.m_IsPersistent), m_Layer(other.m_Layer), m_ID(other.m_ID),
		  m_Components(std::move(other.m_Components)), m_ComponentIndices(std::move(other.m_ComponentIndices)),
		  m_ComponentMap(std::move(other.m_ComponentMap)), m_ComponentMask(other.m_ComponentMask)
	{
		// Обновляем владельца у всех компонентов (они теперь ссылаются на новый адрес Entity)
		for (auto* component : m_Components)
			if (component)
				component->SetOwner(this);
	}

	// Оператор перемещения
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

			for (auto* component : m_Components)
				if (component)
					component->SetOwner(this);
		}
		return *this;
	}

	void Update(float deltaTime)
	{
		if (!m_IsActive)
			return;

		for (auto* component : m_Components)
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

				// Удаляем из менеджера по сохраненному индексу
				s_ComponentManager.DestroyComponent(typeID, m_ComponentIndices[i]);
			}
		}

		m_Components.clear();
		m_ComponentIndices.clear();
		m_ComponentMap.clear();
		m_ComponentMask.reset();
	}

	// --- Component API ---

	template <typename T, typename... Args> Entity& Add(Args&&... args)
	{
		AddComponent<T>(std::forward<Args>(args)...);
		return *this;
	}

	template <typename T> T* Get() const
	{
		auto it = m_ComponentMap.find(ComponentTypeInfo::GetID<T>());
		return it != m_ComponentMap.end() ? static_cast<T*>(it->second) : nullptr;
	}

	template <typename T> bool Has() const
	{
		return m_ComponentMask[ComponentTypeInfo::GetID<T>()];
	}

	template <typename T> void Remove()
	{
		const ComponentTypeID typeID = ComponentTypeInfo::GetID<T>();
		auto it = m_ComponentMap.find(typeID);
		if (it != m_ComponentMap.end())
		{
			for (size_t i = 0; i < m_Components.size(); ++i)
			{
				if (m_Components[i] == it->second) // Сравниваем указатели
				{
					m_Components[i]->OnDestroy();
					s_ComponentManager.DestroyComponent(typeID, m_ComponentIndices[i]);

					m_Components.erase(m_Components.begin() + i);
					m_ComponentIndices.erase(m_ComponentIndices.begin() + i);
					break;
				}
			}

			m_ComponentMap.erase(it);
			m_ComponentMask.reset(typeID);
		}
	}

	template <typename T, typename... Args> T* GetOrAdd(Args&&... args)
	{
		if (T* component = Get<T>())
			return component;
		return AddComponent<T>(std::forward<Args>(args)...);
	}

	template <typename... Components> bool HasAll() const
	{
		return (Has<Components>() && ...);
	}

	template <typename... Components> bool HasAny() const
	{
		return (Has<Components>() || ...);
	}

	template <typename T> std::vector<T*> GetAll() const
	{
		std::vector<T*> result;
		for (auto* component : m_Components)
		{
			if (auto casted = dynamic_cast<T*>(component))
				result.push_back(casted);
		}
		return result;
	}

	// Геттеры/Сеттеры
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

	template <typename Visitor> void ForEachComponent(Visitor&& visitor)
	{
		for (auto* component : m_Components)
			if (component && component->IsActive())
				visitor(*component);
	}

	// Статическая очистка
	static void ReleaseAllComponents()
	{
		s_ComponentManager.Clear();
	}

  private:
	template <typename T, typename... Args> T* AddComponent(Args&&... args)
	{
		const ComponentTypeID typeID = ComponentTypeInfo::GetID<T>();

		if (m_ComponentMap.find(typeID) != m_ComponentMap.end())
		{
			assert(false && "Component of this type already exists!");
			return Get<T>();
		}

		// FIX: Получаем структуру с указателем и реальным индексом
		auto result = s_ComponentManager.CreateComponent<T>(std::forward<Args>(args)...);
		T* component = result.Ptr;

		component->SetOwner(this);

		m_Components.push_back(component);
		m_ComponentIndices.push_back(result.PoolIndex); // FIX: Сохраняем реальный индекс
		m_ComponentMap[typeID] = component;
		m_ComponentMask.set(typeID);

		component->OnCreate();
		if (component->IsActive())
			component->OnEnable();

		return component;
	}
};

// =========================================================
// 6. ВСПОМОГАТЕЛЬНЫЕ ШАБЛОНЫ
// =========================================================

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

	template <typename T, typename Func> EntityBuilder& AddComponent(Func&& configurator)
	{
		T* component = m_Entity->Add<T>();
		configurator(*component);
		return *this;
	}

	std::unique_ptr<Entity> Build()
	{
		return std::move(m_Entity);
	}

	Entity* Get()
	{
		return m_Entity.get();
	}
};

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

// =========================================================
// 7. INLINE РЕАЛИЗАЦИЯ МЕТОДОВ IComponent
// =========================================================

inline Entity* IComponent::GetOwner() const
{
	return m_Owner;
}

inline bool IComponent::IsActive() const
{
	return m_Active && (m_Owner ? m_Owner->IsActive() : true);
}

inline void IComponent::SetActive(bool active)
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

} // namespace Core::ECS
