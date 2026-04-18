#pragma once

class CEngine
{
public:
	CEngine();
	~CEngine();

	Initialize();
	Run();
	Shutdown();

private:
	bool m_bIsRunning = false;
};
