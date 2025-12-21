///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// ImGui implementation
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "imgui_api.h"
#include "Render.h"		// Доступ к Rendeructor и Window
#include "filesystem.h" // Доступ к FS
#include <d3d11.h>		// Нужен для приведения типов (ID3D11Device)
///////////////////////////////////////////////////////////////
namespace Core
{

void CImguiAPI::Initialize()
{
	if (m_Initialized)
		return;

	Log("Initializing ImGuiAPI...");

	// 1. Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	// io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Если нужна докинг система

	// 2. Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// 3. Получаем зависимости от движка
	auto* window = Core::CRender::GetInstance().GetWindow()->GetSDLWindow();

	// ВАЖНО: Получаем сырые указатели из нашего Rendeructor
	// Мы предполагаем, что ты добавил методы GetDevice/GetContext в Rendeructor, как обсуждали выше
	ID3D11Device* device = (ID3D11Device*)Core::RenderBackend.GetDevice();
	ID3D11DeviceContext* context = (ID3D11DeviceContext*)Core::RenderBackend.GetContext();

	if (!window || !device || !context)
	{
		Core::Debug::ErrorHandler::handleCriticalError(
			"Failed to initialize ImGui: Window or Graphics Device is null!");
		return;
	}

	// 4. Init Platform/Renderer backends
	ImGui_ImplSDL2_InitForD3D(window);
	ImGui_ImplDX11_Init(device, context);

	// 5. Load Fonts
	try
	{
		// --- Letterica (Как было раньше) ---
		std::string lettericaPath = FS.GetGameResourcesPath({"fonts", "letterica-3.ttf"}).string();

		if (FS.FileExists(lettericaPath))
		{
			font_letterica = io.Fonts->AddFontFromFileTTF(lettericaPath.c_str(), 14.0f);
			font_letterica_small = io.Fonts->AddFontFromFileTTF(lettericaPath.c_str(), 10.0f);
			font_letterica_medium = io.Fonts->AddFontFromFileTTF(lettericaPath.c_str(), 14.0f);
			font_letterica_big = io.Fonts->AddFontFromFileTTF(lettericaPath.c_str(), 18.0f);
		}
		else
		{
			Log("Warning: Font not found at %s", lettericaPath.c_str());
			io.Fonts->AddFontDefault(); // Фолбек, если основной шрифт не найден
		}

		// --- Maven Pro (Все начертания) ---

		// 1. Regular
		std::string mavenRegularPath = FS.GetGameResourcesPath({"fonts", "MavenPro-Regular.ttf"}).string();
		if (FS.FileExists(mavenRegularPath))
		{
			font_maven_pro_regular = io.Fonts->AddFontFromFileTTF(mavenRegularPath.c_str(), 14.0f);
		}
		else
		{
			Log("Warning: MavenPro-Regular not found at %s", mavenRegularPath.c_str());
		}

		// 2. Medium
		std::string mavenMediumPath = FS.GetGameResourcesPath({"fonts", "MavenPro-Medium.ttf"}).string();
		if (FS.FileExists(mavenMediumPath))
		{
			font_maven_pro_medium = io.Fonts->AddFontFromFileTTF(mavenMediumPath.c_str(), 14.0f);
		}
		else
		{
			Log("Warning: MavenPro-Medium not found at %s", mavenMediumPath.c_str());
		}

		// 3. Bold
		std::string mavenBoldPath = FS.GetGameResourcesPath({"fonts", "MavenPro-Bold.ttf"}).string();
		if (FS.FileExists(mavenBoldPath))
		{
			font_maven_pro_bold = io.Fonts->AddFontFromFileTTF(mavenBoldPath.c_str(), 14.0f);
		}
		else
		{
			Log("Warning: MavenPro-Bold not found at %s", mavenBoldPath.c_str());
		}

		// 4. Black
		std::string mavenBlackPath = FS.GetGameResourcesPath({"fonts", "MavenPro-Black.ttf"}).string();
		if (FS.FileExists(mavenBlackPath))
		{
			font_maven_pro_black = io.Fonts->AddFontFromFileTTF(mavenBlackPath.c_str(), 14.0f);
		}
		else
		{
			Log("Warning: MavenPro-Black not found at %s", mavenBlackPath.c_str());
		}
	}
	catch (const std::exception& e)
	{
		Log("Exception while loading fonts: %s", e.what());
		io.Fonts->AddFontDefault();
	}
	catch (...)
	{
		Log("Unknown error loading fonts");
		io.Fonts->AddFontDefault();
	}

	m_Initialized = true;
	Log("ImGuiAPI Initialized Successfully");
}

void CImguiAPI::ProcessEvent(const SDL_Event* event)
{
	if (m_Initialized)
	{
		ImGui_ImplSDL2_ProcessEvent(event);
	}
}

void CImguiAPI::OnFrameBegin()
{
	if (!m_Initialized)
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void CImguiAPI::RenderFrame()
{
	if (!m_Initialized)
		return;

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImguiAPI::Destroy()
{
	if (!m_Initialized)
		return;

	Log("Destroying ImGuiAPI...");

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	m_Initialized = false;
}

CImguiAPI::~CImguiAPI()
{
	Destroy();
}

void CImguiAPI::HideCursor()
{
	ImGui::SetMouseCursor(ImGuiMouseCursor_None);
}

void CImguiAPI::ShowCursor()
{
	ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
}

 ImGuiContext* Core_GetImGuiContext()
{
	return ImGui::GetCurrentContext();
}

 ImPlotContext* Core_GetImPlotContext()
{
	return ImPlot::GetCurrentContext();
}

void Core_GetImGuiAllocators(ImGuiMemAllocFunc* allocFunc, ImGuiMemFreeFunc* freeFunc, void** userData)
{
	ImGui::GetAllocatorFunctions(allocFunc, freeFunc, userData);
}

} // namespace Core