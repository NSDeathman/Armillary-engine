///////////////////////////////////////////////////////////////
// Created: 21.09.2025
// Updated: [Текущая дата]
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
//            _   _
//           (.)_(.)
//        _ (   _   ) _
//       / \/`-----'\/ \
//     __\ ( (     ) ) /__
//     )   /\ \._./ /\   (
// jgs   )_/ /|\   /|\ \_(
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "Core.h"
#include <functional>
#include <vector>
#include <string>
///////////////////////////////////////////////////////////////
CORE_BEGIN
// Структура для таймеров с коллбэками
struct Timer
{
	std::string id;
	float duration;
	float elapsed;
	bool loop;
	bool active;
	std::function<void()> onComplete;
	std::function<void(float)> onUpdate;
};

// Структура для временной шкалы
struct TimeScale
{
	std::string id;
	float scale;
	float duration;
	bool active;
};

class CTimeSystem
{
  public:
	CTimeSystem() = default;
	~CTimeSystem() = default;

	// Основные методы
	void Initialize();
	void Update();
	void Destroy();

	// Свойства времени
	float GetDeltaTime() const;
	float GetTimeSinceStart() const;
	float GetTotalTime() const; // Псевдоним для GetTimeSinceStart
	int GetFrameCount() const;

	// Временные масштабы (для slow motion и т.д.)
	float GetGlobalTimeScale() const;
	void SetGlobalTimeScale(float scale);
	void ResetGlobalTimeScale();

	// Управление FPS
	void SetTargetFPS(int fps);
	int GetTargetFPS() const;
	int GetCurrentFPS() const;
	float GetAverageFPS() const;
	float GetMinFPS() const;
	float GetMaxFPS() const;

	// Ограничение FPS
	void EnableFPSCap(bool enable);
	bool IsFPSCapEnabled() const;

	// ========== НОВЫЕ МЕТОДЫ ДЛЯ ЭФФЕКТОВ КАМЕРЫ ==========

	// Таймеры с коллбэками (для эффектов тряски и т.д.)
	std::string CreateTimer(float duration, std::function<void()> onComplete = nullptr,
							std::function<void(float)> onUpdate = nullptr, bool loop = false, bool autoStart = true);

	bool StartTimer(const std::string& id);
	bool StopTimer(const std::string& id);
	bool ResetTimer(const std::string& id);
	bool RemoveTimer(const std::string& id);
	float GetTimerProgress(const std::string& id) const;
	bool IsTimerActive(const std::string& id) const;

	// Временные эффекты (slow motion, пауза)
	void SetTimeScale(const std::string& id, float scale, float duration = 0.0f);
	float GetTimeScale(const std::string& id) const;
	void RemoveTimeScale(const std::string& id);

	// Для эффектов камеры
	float GetSmoothDeltaTime() const; // Дельта-тайм с примененным временным масштабом
	float GetFixedDeltaTime() const;	// Фиксированный дельта-тайм для физики
	float GetUnscaledDeltaTime() const; // Дельта-тайм без временного масштаба

	// Осцилляторы для эффектов тряски/покачивания
	float GetSineWave(float frequency, float amplitude = 1.0f, float phase = 0.0f) const;
	float GetCosineWave(float frequency, float amplitude = 1.0f, float phase = 0.0f) const;
	float GetTriangleWave(float frequency, float amplitude = 1.0f) const;
	float GetSawtoothWave(float frequency, float amplitude = 1.0f) const;

	// Шум для эффектов камеры (тряска, дрейф и т.д.)
	float GetPerlinNoise2D(float x, float y) const;
	float GetPerlinNoise3D(float x, float y, float z) const;
	float GetValueNoise2D(float x, float y) const;

	// Время в циклах (для периодических эффектов)
	float GetCyclicTime(float cycleDuration) const;
	float GetPingPongTime(float duration) const;

	// Утилиты для интерполяции
	float GetLerpFactor(float smoothTime, float deltaTime) const;
	float GetDampedSpring(float current, float target, float& velocity, float smoothTime, float deltaTime) const;

	// Статистика производительности
	struct PerformanceStats
	{
		float frameTime;
		float updateTime;
		float renderTime;
		float inputTime;
		float physicsTime;
		float frameTimePercentiles[5]; // 50th, 75th, 90th, 95th, 99th
	};

	PerformanceStats GetPerformanceStats() const;
	void ResetPerformanceStats();

	// Отладочная информация
	void EnableProfiling(bool enable);
	bool IsProfilingEnabled() const;

  private:

	// Реализация (PIMPL)
	class Impl;
	Impl* m_Impl;
};
CORE_END
///////////////////////////////////////////////////////////////
