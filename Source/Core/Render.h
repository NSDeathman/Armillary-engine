///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "DebugAPI.h"
#include "Window.h"
///////////////////////////////////////////////////////////////
namespace Core
{
extern Rendeructor RenderBackend;

struct CORE_API RenderConfig
{
	int Width = 1920;
	int Height = 1080;
	ScreenMode ScreenMode = ScreenMode::Windowed;
	RenderAPI API = RenderAPI::DirectX11;
	void* WindowHandle = nullptr;
	const char* WindowTitle = "Armillary Engine";
};

class CORE_API CRender : public Patterns::Singleton<CRender>
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

  private:
	bool CreateRenderWindow();
	BackendConfig RenderConfigToBackendConfig(const RenderConfig& config);

	CWindow m_Window;
	RenderConfig m_Config;
	bool m_Initialized = false;
};

} // namespace Core
///////////////////////////////////////////////////////////////
#define Renderer Core::CRender::GetInstance()
///////////////////////////////////////////////////////////////