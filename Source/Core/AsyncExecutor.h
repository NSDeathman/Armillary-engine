///////////////////////////////////////////////////////////////
// Created: 15.09.2025
// Author: DeepSeek, NS_Deathman
// Multithreaded async function executor
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <vector>
#include "CoreMacros.h"
///////////////////////////////////////////////////////////////
CORE_BEGIN
class CAsyncExecutor
{
	public:
	CAsyncExecutor() = default;
	~CAsyncExecutor() = default;

	// Инициализация с настраиваемым количеством потоков
	void Initialize(size_t threadCount = 0);
	void Destroy();

	// Запуск функции без возвращаемого значения
	void Execute(std::function<void()> function);

	// Запуск функции с возвращаемым значением (возвращает future)
	template <typename F, typename... Args> auto Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

	// Ожидание завершения всех задач
	void WaitAll();

	// Количество pending задач
	size_t GetPendingCount() const;

	// Состояние исполнителя
	bool IsInitialized() const
	{
		return m_Initialized;
	}
	bool IsStopped() const
	{
		return stop;
	}

	// Остановка executor (завершает потоки после выполнения текущих задач)
	void Stop();

	private:
	void WorkerThread();

	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	mutable std::mutex queueMutex;
	std::condition_variable condition;
	std::atomic<bool> stop{false};
	std::atomic<size_t> pendingTasks{0};
	std::atomic<bool> m_Initialized{false};
};
CORE_END
///////////////////////////////////////////////////////////////
#define ASYNC_EXEC(fn) ASYNC.Execute(fn)
#define ASYNC_SUBMIT(fn, ...) ASYNC.Submit(fn, __VA_ARGS__)
///////////////////////////////////////////////////////////////
