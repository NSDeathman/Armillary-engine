#pragma once
#include "GameObject.h"
#include "StaticModel.h"
#include <Rendeructor/RendeructorDefines.h>
#include <memory>

namespace Core::World
{
class CStaticObject : public CGameObject
{
  public:
	CStaticObject(const std::string& name, std::shared_ptr<StaticModel> model);

	void SetShader(const ShaderPass& pass);

	void Draw() override;

	std::shared_ptr<StaticModel> GetModel() const
	{
		return m_Model;
	}

  private:
	std::shared_ptr<StaticModel> m_Model;
	ShaderPass m_ObjectShader;
	bool m_HasShader = false;
};
} // namespace Core::World
