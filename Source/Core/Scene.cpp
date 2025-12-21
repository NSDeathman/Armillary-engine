#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"
#include "Render.h"
#include "Camera.h"

#include <vector>

namespace Core::World
{
struct SceneInternals
{
	bool Initialized = false;
	std::vector<std::shared_ptr<CGameObject>> AllObjects;
};

CScene::CScene() : m_Impl(std::make_unique<SceneInternals>())
{
}
CScene::~CScene() = default;
CScene::CScene(CScene&&) noexcept = default;
CScene& CScene::operator=(CScene&&) noexcept = default;

bool CScene::Initialize()
{
	if (m_Impl->Initialized)
		return true;
	m_Impl->Initialized = true;
	return true;
}

void CScene::Cleanup()
{
	if (m_Impl)
		m_Impl->AllObjects.clear();
}

void CScene::AddObject(std::shared_ptr<CGameObject> object)
{
	if (m_Impl && object)
		m_Impl->AllObjects.push_back(object);
}

void CScene::RemoveObject(const std::string& name)
{
	if (!m_Impl)
		return;
	std::erase_if(m_Impl->AllObjects, [&](auto& obj) { return obj->GetName() == name; });
}

void CScene::Update(float deltaTime)
{
	if (!m_Impl)
		return;
	for (auto& obj : m_Impl->AllObjects)
		if (obj)
			obj->Update(deltaTime);
}

void CScene::Render()
{
	if (!m_Impl || !m_Impl->Initialized)
		return;

	for (auto& obj : m_Impl->AllObjects)
	{
		obj->Draw();
	}
}
} // namespace Core::World
