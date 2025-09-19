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
///////////////////////////////////////////////////////////////
class CAsyncExecutor
{
  public:
	CAsyncExecutor();
	~CAsyncExecutor();

	// ������� ����������� � �����������
	CAsyncExecutor(const CAsyncExecutor&) = delete;
	CAsyncExecutor(CAsyncExecutor&&) = delete;
	CAsyncExecutor& operator=(const CAsyncExecutor&) = delete;
	CAsyncExecutor& operator=(CAsyncExecutor&&) = delete;

	// ������ ������� ��� ������������� ��������
	void Execute(std::function<void()> function);

	// ������ ������� � ������������ ��������� (���������� future)
	template <typename F, typename... Args> auto Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

	// �������� ���������� ���� �����
	void WaitAll();

	// ���������� pending �����
	size_t GetPendingCount() const;

	// ��������� executor (��������� ������ ����� ���������� ������� �����)
	void Stop();

  private:
	void WorkerThread();

	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	mutable std::mutex queueMutex;
	std::condition_variable condition;
	std::atomic<bool> stop{false};
	std::atomic<size_t> pendingTasks{0};
};
///////////////////////////////////////////////////////////////
extern CAsyncExecutor* AsyncExecutor;
///////////////////////////////////////////////////////////////
