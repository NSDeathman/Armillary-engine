#pragma once

namespace Armillary
{

	class Engine
	{
	public:
		Engine();
		~Engine();

		bool Initialize();
		void Run();
		void Shutdown();

	private:
		bool m_bIsRunning = false;
	};

} // namespace Armillary