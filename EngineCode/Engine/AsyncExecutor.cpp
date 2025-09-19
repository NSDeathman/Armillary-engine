///////////////////////////////////////////////////////////////
// Created: 15.09.2025
// Author: DeepSeek, NS_Deathman
// Multithreaded async function executor
///////////////////////////////////////////////////////////////
#include "AsyncExecutor.h"
#include <stdexcept>
///////////////////////////////////////////////////////////////
CAsyncExecutor::CAsyncExecutor()
{
	int threadCount = std::thread::hardware_concurrency();

	if (threadCount == 0)
	{
		throw std::invalid_argument("Thread count must be greater than 0");
	}

	workers.reserve(threadCount);
	for (size_t i = 0; i < threadCount; ++i)
	{
		workers.emplace_back(&CAsyncExecutor::WorkerThread, this);
	}
}

CAsyncExecutor::~CAsyncExecutor()
{
	Stop();
}

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
			catch (...)
			{
				// Игнорируем исключения в задачах
			}
			pendingTasks--;
		}
	}
}

void CAsyncExecutor::Execute(std::function<void()> function)
{
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

template <typename F, typename... Args>
auto CAsyncExecutor::Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{
	using ReturnType = decltype(f(args...));

	auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

	std::future<ReturnType> result = task->get_future();

	Execute([task]() { (*task)(); });

	return result;
}

void CAsyncExecutor::WaitAll()
{
	while (pendingTasks > 0)
	{
		std::this_thread::yield();
	}
}

size_t CAsyncExecutor::GetPendingCount() const
{
	return pendingTasks;
}

void CAsyncExecutor::Stop()
{
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
}
///////////////////////////////////////////////////////////////
CAsyncExecutor* AsyncExecutor = nullptr;
///////////////////////////////////////////////////////////////
