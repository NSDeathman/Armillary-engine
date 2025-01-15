///////////////////////////////////////////////////////////////
// From optick mt demo
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <iostream>
#include <array>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <stdio.h>
#include <functional>
///////////////////////////////////////////////////////////////
class CScheduler
{
	struct Context
	{
		volatile bool destroying = false;
		std::thread worker;
		std::queue<std::function<void()>> jobQueue;
		std::mutex queueMutex;
		std::condition_variable condition;

		void Update()
		{
			OPTICK_THREAD("Worker");
			while (true)
			{
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(queueMutex);
					condition.wait(lock, [this] { return !jobQueue.empty() || destroying; });
					if (destroying)
						break;
					job = jobQueue.front();
				}
				job();
				{
					std::lock_guard<std::mutex> lock(queueMutex);
					jobQueue.pop();
					condition.notify_one();
				}
			}
		}

		Context() : worker(std::thread(&Context::Update, this))
		{
		}
		~Context()
		{
			if (worker.joinable())
			{
				destroying = true;
				condition.notify_one();
				worker.join();
			}
		}

		void Add(std::function<void()> function)
		{
			std::lock_guard<std::mutex> lock(queueMutex);
			jobQueue.push(std::move(function));
			condition.notify_one();
		}
	};

	std::array<Context, 4> threads;

  public:
	void Add(std::function<void()> function)
	{
		int index = rand() % threads.size();
		threads[index].Add(function);
	}
};
///////////////////////////////////////////////////////////////
CScheduler Scheduler;
///////////////////////////////////////////////////////////////
