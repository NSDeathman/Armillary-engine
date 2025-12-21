#pragma once

#include <memory>
#include <string>
#include <MathAPI/MathAPI.h>

namespace Core::World
{
class CGameObject;
struct SceneInternals;

struct SceneBufferData
{
	Math::float4x4 ViewProjection;
	Math::float3 CameraPosition;
	float Time;
};

class CScene
{
  public:
	CScene();

	~CScene();

	CScene(const CScene&) = delete;
	CScene& operator=(const CScene&) = delete;

	CScene(CScene&& other) noexcept;
	CScene& operator=(CScene&& other) noexcept;

	bool Initialize();
	void Cleanup();

	void AddObject(std::shared_ptr<CGameObject> object);
	void RemoveObject(const std::string& name);

	void Update(float deltaTime);

	void Render();

  private:
	std::unique_ptr<SceneInternals> m_Impl;
};
} // namespace Core::World
