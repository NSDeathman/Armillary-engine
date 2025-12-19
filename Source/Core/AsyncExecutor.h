///////////////////////////////////////////////////////////////
// Created: 15.09.2025
// Author: DeepSeek, NS_Deathman
// Multithreaded async function executor
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Core.h"
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
namespace Core
{
	class CORE_API CAsyncExecutor
	{
	  public:
		// Singleton методы
		static CAsyncExecutor& GetInstance();

		// Удаляем копирование и перемещение
		CAsyncExecutor(const CAsyncExecutor&) = delete;
		CAsyncExecutor(CAsyncExecutor&&) = delete;
		CAsyncExecutor& operator=(const CAsyncExecutor&) = delete;
		CAsyncExecutor& operator=(CAsyncExecutor&&) = delete;

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
		CAsyncExecutor();
		~CAsyncExecutor();

		void WorkerThread();

		std::vector<std::thread> workers;
		std::queue<std::function<void()>> tasks;
		mutable std::mutex queueMutex;
		std::condition_variable condition;
		std::atomic<bool> stop{false};
		std::atomic<size_t> pendingTasks{0};
		std::atomic<bool> m_Initialized{false};
	};
} // namespace Core
///////////////////////////////////////////////////////////////
#define ASYNC CAsyncExecutor::GetInstance()
#define ASYNC_API_INIT() ASYNC.Initialize(0)
#define ASYNC_API_DESTROY() ASYNC.Destroy()
#define ASYNC_EXEC(fn) ASYNC.Execute(fn)
#define ASYNC_SUBMIT(fn, ...) ASYNC.Submit(fn, __VA_ARGS__)
///////////////////////////////////////////////////////////////
//                  _uP~"b          d"u,
//                 dP'   "b       ,d"  "o
//                d"    , `b     d"'    "b
//               l] [    " `l,  d"       lb
//               Ol ?     "  "b`"=uoqo,_  "l
//             ,dBb "b        "b,    `"~~TObup,_
//           ,d" (db.`"         ""     "tbc,_ `~"Yuu,_
//         .d" l`T'  '=                      ~     `""Yu,
//       ,dO` gP,                           `u,   b,_  "b7
//      d?' ,d" l,                           `"b,_ `~b  "1
//    ,8i' dl   `l                 ,ggQOV",dbgq,._"  `l  lb
//   .df' (O,    "             ,ggQY"~  , @@@@@d"bd~  `b "1
//  .df'   `"           -=@QgpOY""     (b  @@@@P db    `Lp"b,
// .d(                  _               "ko "=d_,Q`  ,_  "  "b,
// Ql         .         `"qo,._          "tQo,_`""bo ;tb,    `"b,
//(qQ         |L           ~"QQQgggc,_.,dObc,opooO  `"~~";.   __,7,
//`qp         t\io,_           `~"TOOggQV""""        _,dg,_ =PIQHib.
// `qp        `Q["tQQQo,_                          ,pl{QOP"'   7AFR`
//   `         `tb  '""tQQQg,_             p" "b   `       .;-.`Vl'
//              "Yb      `"tQOOo,__    _,edb    ` .__   /`/'|  |b;=;.__
//                            `"tQQQOOOOP""        `"\QV;qQObob"`-._`\_~~-._
//                                 """"    ._        /   | |oP"\_   ~\ ~\_  ~\
//                                         `~"\ic,qggddOOP"|  |  ~\   `\  ~-._
//                                           ,qP`"""|"   | `\ `;   `\   `\
//                                _        _,p"     |    |   `\`;    |    |
//                                 "boo,._dP"       `\_  `\    `\|   `\   ;
//                                  `"7tY~'            `\  `\    `|_   |
///////////////////////////////////////////////////////////////