///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "DebugAPI.h"
#include "Window.h"
///////////////////////////////////////////////////////////////
namespace Core
{
namespace World
{
class CCamera;
class CScene;
}

extern Rendeructor RenderBackend;

struct  RenderConfig
{
	int Width = 1920;
	int Height = 1080;
	ScreenMode ScreenMode = ScreenMode::Windowed;
	RenderAPI API = RenderAPI::DirectX11;
	void* WindowHandle = nullptr;
	const char* WindowTitle = "Armillary Engine";
};

class CRender : public Patterns::Singleton<CRender>
{
	friend class Patterns::Singleton<CRender>;

  public:
	CRender();
	virtual ~CRender();

	bool Initialize(const RenderConfig& config);
	void Shutdown();
	void Resize(int width, int height);
	void BeginFrame();
	void EndFrame();
	void DrawFrame();

	bool IsInitialized() const
	{
		return m_Initialized;
	}

	CWindow* GetWindow()
	{
		return &m_Window;
	}

	RenderConfig GetConfig() const
	{
		return m_Config;
	}

	void SetCurrentScene(std::shared_ptr<Core::World::CScene> scene);
	void SetCurrentCamera(std::shared_ptr<Core::World::CCamera> camera);

	std::shared_ptr<Core::World::CCamera> GetCurrentCamera()
	{
		return m_ActiveCamera;
	}

	Math::float2 GetScreenResolution()
	{
		return Math::float2(m_Config.Width, m_Config.Height);
	}

  private:
	bool CreateRenderWindow();
	BackendConfig RenderConfigToBackendConfig(const RenderConfig& config);

	CWindow m_Window;
	RenderConfig m_Config;
	bool m_Initialized = false;

	std::shared_ptr<Core::World::CScene> m_ActiveScene = nullptr;
	std::shared_ptr<Core::World::CCamera> m_ActiveCamera = nullptr;
};

} // namespace Core
///////////////////////////////////////////////////////////////
#define Renderer Core::CRender::GetInstance()
///////////////////////////////////////////////////////////////