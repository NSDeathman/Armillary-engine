#pragma once
#include <string>
#include <MathAPI/MathAPI.h>
#include "Transform.h"
#include "CoreAPI.h"

namespace Core::World
{
enum class ObjectType
{
	Base,
	Static,
	Dynamic
};

class CGameObject
{
  public:
	CGameObject(const std::string& name) : m_Name(name), m_Type(ObjectType::Base)
	{
	}
	virtual ~CGameObject() = default;

	virtual void Update(float deltaTime)
	{
	}

	virtual void Draw()
	{
	}

	Transform& GetTransform()
	{
		return m_Transform;
	}

	ObjectType GetType() const
	{
		return m_Type;
	}
	const std::string& GetName() const
	{
		return m_Name;
	}

  protected:
	std::string m_Name;
	Transform m_Transform;
	ObjectType m_Type;
};
} // namespace Core::World
