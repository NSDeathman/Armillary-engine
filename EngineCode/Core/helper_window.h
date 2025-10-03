///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// ImGui helping window
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
extern bool g_bNeedRestart;
extern bool g_bWireframeMode;
///////////////////////////////////////////////////////////////
class CHelperWindow
{
  private:
	bool m_bNeedDraw;
	bool m_bNeedQuitToMainMenu;

	bool m_bNeedDrawSettings;
	bool m_bNeedDrawProfilingSettings;

	bool m_bNeedLeaveToScene;

  public:
	CHelperWindow();
	~CHelperWindow() = default;

	void DrawSettings();
	void LoadSettings();
	void SaveSettings();
	void DrawProfilingSettings();

	void Draw();
	void Show();
	void Hide();

	bool NeedQuitToMainMenu();
	bool NeedLeaveToScene();
	void LeavingToSceneIsDone();
	void QuitingToMainMenuIsDone();
};
///////////////////////////////////////////////////////////////
extern CHelperWindow* HelperWindow;
///////////////////////////////////////////////////////////////
