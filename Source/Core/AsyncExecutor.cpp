///////////////////////////////////////////////////////////////
// Created: 15.09.2025
// Author: DeepSeek, NS_Deathman
// Multithreaded async function executor
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AsyncExecutor.h"
#include "log.h"
#include <stdexcept>
///////////////////////////////////////////////////////////////
namespace Core
{
	// Singleton instance
	CAsyncExecutor& CAsyncExecutor::GetInstance()
	{
		static CAsyncExecutor instance;
		return instance;
	}

	///////////////////////////////////////////////////////////////
	CAsyncExecutor::CAsyncExecutor()
	{
	}

	CAsyncExecutor::~CAsyncExecutor()
	{
		Destroy();
	}

	///////////////////////////////////////////////////////////////
	void CAsyncExecutor::Initialize(size_t threadCount)
	{
		if (m_Initialized)
		{
			Log("AsyncExecutor already initialized!");
			return;
		}

		if (threadCount == 0)
		{
			threadCount = std::thread::hardware_concurrency();
			if (threadCount == 0)
			{
				threadCount = 4; // Fallback значение
				Log("Using fallback thread count: %zu", threadCount);
			}
		}

		try
		{
			stop = false;
			workers.reserve(threadCount);

			for (size_t i = 0; i < threadCount; ++i)
			{
				workers.emplace_back(&CAsyncExecutor::WorkerThread, this);
			}

			m_Initialized = true;
			Log("AsyncExecutor initialized with %zu threads\n", threadCount);
		}
		catch (const std::exception& e)
		{
			Log("Failed to initialize AsyncExecutor: %s\n", e.what());
			throw;
		}
	}

	///////////////////////////////////////////////////////////////
	void CAsyncExecutor::Destroy()
	{
		if (m_Initialized)
		{
			Log("Destroying AsyncExecutor...");
			Stop();
			m_Initialized = false;
		}
	}

	///////////////////////////////////////////////////////////////
	void CAsyncExecutor::WorkerThread()
	{
		while (!stop)
		{
			std::function<void()> task;

			{
				std::unique_lock<std::mutex> lock(queueMutex);
				condition.wait(lock, [this] { return stop || !tasks.empty(); });

				if (stop && tasks.empty())
				{
					return;
				}

				if (!tasks.empty())
				{
					task = std::move(tasks.front());
					tasks.pop();
				}
			}

			if (task)
			{
				try
				{
					task();
				}
				catch (const std::exception& e)
				{
					Log("Exception in async task: %s", e.what());
				}
				catch (...)
				{
					Log("Unknown exception in async task");
				}
				pendingTasks--;
			}
		}
	}

	///////////////////////////////////////////////////////////////
	void CAsyncExecutor::Execute(std::function<void()> function)
	{
		if (!m_Initialized)
		{
			throw std::runtime_error("AsyncExecutor not initialized");
		}

		if (!function)
		{
			throw std::invalid_argument("Function cannot be null");
		}

		{
			std::lock_guard<std::mutex> lock(queueMutex);
			tasks.push(std::move(function));
			pendingTasks++;
		}
		condition.notify_one();
	}

	///////////////////////////////////////////////////////////////
	template <typename F, typename... Args>
	auto CAsyncExecutor::Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
	{
		using ReturnType = decltype(f(args...));

		if (!m_Initialized)
		{
			throw std::runtime_error("AsyncExecutor not initialized");
		}

		auto task =
			std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<ReturnType> result = task->get_future();

		Execute([task]() { (*task)(); });

		return result;
	}

	template CORE_API std::future<void> CAsyncExecutor::Submit(std::function<void()>&&);
	template CORE_API std::future<int> CAsyncExecutor::Submit(std::function<int()>&&);
	template CORE_API std::future<bool> CAsyncExecutor::Submit(std::function<bool()>&&);
	template CORE_API std::future<std::string> CAsyncExecutor::Submit(std::function<std::string()>&&);

	///////////////////////////////////////////////////////////////
	void CAsyncExecutor::WaitAll()
	{
		if (!m_Initialized)
			return;

		while (pendingTasks > 0)
		{
			std::this_thread::yield();
		}
	}

	///////////////////////////////////////////////////////////////
	size_t CAsyncExecutor::GetPendingCount() const
	{
		return pendingTasks;
	}

	///////////////////////////////////////////////////////////////
	void CAsyncExecutor::Stop()
	{
		if (!m_Initialized)
			return;

		stop = true;
		condition.notify_all();

		for (std::thread& worker : workers)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}

		workers.clear();

		std::lock_guard<std::mutex> lock(queueMutex);
		while (!tasks.empty())
		{
			tasks.pop();
			pendingTasks--;
		}

		Log("AsyncExecutor stopped");
	}
} // namespace Core
///////////////////////////////////////////////////////////////
