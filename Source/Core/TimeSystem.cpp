///////////////////////////////////////////////////////////////
// Created: 21.09.2025
// Updated: [Текущая дата]
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TimeSystem.h"
#include "log.h"
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>
#include <unordered_map>
///////////////////////////////////////////////////////////////
using namespace Core;
///////////////////////////////////////////////////////////////
// Простая реализация шума Перлина
class PerlinNoise
{
  private:
	static const int permutation[256];
	int p[512];

	static float fade(float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	static float lerp(float t, float a, float b)
	{
		return a + t * (b - a);
	}
	static float grad(int hash, float x, float y, float z)
	{
		int h = hash & 15;
		float u = h < 8 ? x : y;
		float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

  public:
	PerlinNoise()
	{
		for (int i = 0; i < 256; i++)
			p[i] = permutation[i];
		for (int i = 0; i < 256; i++)
			p[256 + i] = p[i];
	}

	float noise(float x, float y, float z) const
	{
		int X = (int)floor(x) & 255;
		int Y = (int)floor(y) & 255;
		int Z = (int)floor(z) & 255;

		x -= floor(x);
		y -= floor(y);
		z -= floor(z);

		float u = fade(x);
		float v = fade(y);
		float w = fade(z);

		int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return lerp(w,
					lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)),
						 lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))),
					lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)),
						 lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}
};

const int PerlinNoise::permutation[256] = {
	151, 160, 137, 91,	90,	 15,  131, 13,	201, 95,  96,  53,	194, 233, 7,   225, 140, 36,  103, 30,	69,	 142,
	8,	 99,  37,  240, 21,	 10,  23,  190, 6,	 148, 247, 120, 234, 75,  0,   26,	197, 62,  94,  252, 219, 203,
	117, 35,  11,  32,	57,	 177, 33,  88,	237, 149, 56,  87,	174, 20,  125, 136, 171, 168, 68,  175, 74,	 165,
	71,	 134, 139, 48,	27,	 166, 77,  146, 158, 231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,	 41,
	55,	 46,  245, 40,	244, 102, 143, 54,	65,	 25,  63,  161, 1,	 216, 80,  73,	209, 76,  132, 187, 208, 89,
	18,	 169, 200, 196, 135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186, 3,	 64,  52,  217, 226, 250,
	124, 123, 5,   202, 38,	 147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,	 16,  58,  17,	182, 189,
	28,	 42,  223, 183, 170, 213, 119, 248, 152, 2,	  44,  154, 163, 70,  221, 153, 101, 155, 167, 43,	172, 9,
	129, 22,  39,  253, 19,	 98,  108, 110, 79,	 113, 224, 232, 178, 185, 112, 104, 218, 246, 97,  228, 251, 34,
	242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,	145, 235, 249, 14,	239, 107, 49,  192, 214, 31,
	181, 199, 106, 157, 184, 84,  204, 176, 115, 121, 50,  45,	127, 4,	  150, 254, 138, 236, 205, 93,	222, 114,
	67,	 29,  24,  72,	243, 141, 128, 195, 78,	 66,  215, 61,	156, 180};

class CTimeSystem::Impl
{
  public:
	// Основные временные переменные
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point lastFrameTime;
	float deltaTime = 0.0f;
	float unscaledDeltaTime = 0.0f;
	float fixedDeltaTime = 1.0f / 60.0f; // Для физики
	int frameCount = 0;

	// Настройки FPS
	int targetFPS = 60;
	int currentFPS = 0;
	bool fpsCapEnabled = true;

	// Временной масштаб
	float globalTimeScale = 1.0f;
	std::vector<TimeScale> timeScales;

	// Таймеры
	std::unordered_map<std::string, Timer> timers;
	std::vector<std::string> timersToRemove;

	// Для расчета FPS
	float fpsAccumulator = 0.0f;
	int fpsFrames = 0;
	std::chrono::high_resolution_clock::time_point lastFPSUpdate;

	// Статистика производительности
	std::vector<float> frameTimes;
	std::chrono::high_resolution_clock::time_point frameStartTime;
	bool profilingEnabled = false;
	float minFPS = 1000.0f;
	float maxFPS = 0.0f;
	float averageFPS = 0.0f;

	// Шум для эффектов
	PerlinNoise perlinNoise;
	std::mt19937 randomEngine;
	std::uniform_real_distribution<float> randomDist;

	Impl() : randomEngine(std::random_device{}()), randomDist(0.0f, 1.0f)
	{
		startTime = std::chrono::high_resolution_clock::now();
		lastFrameTime = startTime;
		lastFPSUpdate = startTime;
		frameStartTime = startTime;

		// Инициализация истории кадров
		frameTimes.reserve(1000);
	}

	// Утилиты
	float GetEffectiveTimeScale() const
	{
		float scale = globalTimeScale;
		for (const auto& ts : timeScales)
		{
			if (ts.active)
				scale *= ts.scale;
		}
		return scale;
	}

	void UpdateTimeScales(float dt)
	{
		for (auto it = timeScales.begin(); it != timeScales.end();)
		{
			if (it->duration > 0.0f)
			{
				it->duration -= dt;
				if (it->duration <= 0.0f)
				{
					it = timeScales.erase(it);
					continue;
				}
			}
			++it;
		}
	}

	void UpdateTimers(float dt)
	{
		for (auto& pair : timers)
		{
			Timer& timer = pair.second;
			if (timer.active)
			{
				timer.elapsed += dt;

				// Вызов коллбэка обновления
				if (timer.onUpdate)
				{
					float progress = timer.elapsed / timer.duration;
					timer.onUpdate(progress);
				}

				// Проверка завершения
				if (timer.elapsed >= timer.duration)
				{
					if (timer.onComplete)
					{
						timer.onComplete();
					}

					if (timer.loop)
					{
						timer.elapsed = 0.0f;
					}
					else
					{
						timer.active = false;
						timersToRemove.push_back(timer.id);
					}
				}
			}
		}

		// Удаление завершенных таймеров
		for (const auto& id : timersToRemove)
		{
			timers.erase(id);
		}
		timersToRemove.clear();
	}

	void UpdatePerformanceStats(float frameTime)
	{
		// Обновление истории кадров
		frameTimes.push_back(frameTime);
		if (frameTimes.size() > 1000)
		{
			frameTimes.erase(frameTimes.begin());
		}

		// Расчет FPS
		float currentFPSValue = 1.0f / frameTime;
		minFPS = std::min(minFPS, currentFPSValue);
		maxFPS = std::max(maxFPS, currentFPSValue);

		// Экспоненциальное скользящее среднее для среднего FPS
		float alpha = 0.1f;
		averageFPS = alpha * currentFPSValue + (1.0f - alpha) * averageFPS;
	}
};

void CTimeSystem::Initialize()
{
	if (!m_Impl)
	{
		m_Impl = new Impl();
		Print("Time system initialized");
	}
}

void CTimeSystem::Destroy()
{
	if (m_Impl)
	{
		delete m_Impl;
		m_Impl = nullptr;
		Print("Time system destroyed");
	}
}

void CTimeSystem::Update()
{
	auto currentTime = std::chrono::high_resolution_clock::now();

	// Расчет delta time без масштаба
	auto delta = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_Impl->lastFrameTime);
	m_Impl->unscaledDeltaTime = delta.count() / 1000000.0f;

	// Применяем временной масштаб
	float effectiveTimeScale = m_Impl->GetEffectiveTimeScale();
	m_Impl->deltaTime = m_Impl->unscaledDeltaTime * effectiveTimeScale;

	m_Impl->lastFrameTime = currentTime;
	m_Impl->frameCount++;

	// Обновление временных масштабов
	m_Impl->UpdateTimeScales(m_Impl->unscaledDeltaTime);

	// Обновление таймеров
	m_Impl->UpdateTimers(m_Impl->deltaTime);

	// Расчет FPS
	m_Impl->fpsFrames++;
	auto fpsDelta = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_Impl->lastFPSUpdate);

	if (fpsDelta.count() >= 1.0f)
	{
		m_Impl->currentFPS = m_Impl->fpsFrames;
		m_Impl->fpsFrames = 0;
		m_Impl->lastFPSUpdate = currentTime;
	}

	// Статистика производительности
	float frameTime = m_Impl->unscaledDeltaTime * 1000.0f; // в миллисекундах
	if (m_Impl->profilingEnabled)
	{
		m_Impl->UpdatePerformanceStats(frameTime);
	}

	// Ограничение FPS
	if (m_Impl->fpsCapEnabled && m_Impl->targetFPS > 0)
	{
		float targetFrameTime = 1.0f / m_Impl->targetFPS;
		if (m_Impl->unscaledDeltaTime < targetFrameTime)
		{
			auto sleepTime =
				std::chrono::microseconds(static_cast<int>((targetFrameTime - m_Impl->unscaledDeltaTime) * 1000000));
			std::this_thread::sleep_for(sleepTime);
		}
	}
}

// ========== БАЗОВЫЕ МЕТОДЫ ==========
float CTimeSystem::GetDeltaTime() const
{
	return m_Impl->deltaTime;
}
float CTimeSystem::GetTimeSinceStart() const
{
	auto currentTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_Impl->startTime);
	return duration.count() / 1000000.0f;
}
float CTimeSystem::GetTotalTime() const
{
	return GetTimeSinceStart();
}
int CTimeSystem::GetFrameCount() const
{
	return m_Impl->frameCount;
}

// ========== ВРЕМЕННЫЕ МАСШТАБЫ ==========
float CTimeSystem::GetGlobalTimeScale() const
{
	return m_Impl->globalTimeScale;
}
void CTimeSystem::SetGlobalTimeScale(float scale)
{
	m_Impl->globalTimeScale = std::max(0.0f, scale);
}
void CTimeSystem::ResetGlobalTimeScale()
{
	m_Impl->globalTimeScale = 1.0f;
}

// ========== УПРАВЛЕНИЕ FPS ==========
void CTimeSystem::SetTargetFPS(int fps)
{
	m_Impl->targetFPS = fps;
}
int CTimeSystem::GetTargetFPS() const
{
	return m_Impl->targetFPS;
}
int CTimeSystem::GetCurrentFPS() const
{
	return m_Impl->currentFPS;
}
float CTimeSystem::GetAverageFPS() const
{
	return m_Impl->averageFPS;
}
float CTimeSystem::GetMinFPS() const
{
	return m_Impl->minFPS;
}
float CTimeSystem::GetMaxFPS() const
{
	return m_Impl->maxFPS;
}

void CTimeSystem::EnableFPSCap(bool enable)
{
	m_Impl->fpsCapEnabled = enable;
}
bool CTimeSystem::IsFPSCapEnabled() const
{
	return m_Impl->fpsCapEnabled;
}

// ========== ТАЙМЕРЫ ==========
std::string CTimeSystem::CreateTimer(float duration, std::function<void()> onComplete,
									 std::function<void(float)> onUpdate, bool loop, bool autoStart)
{
	static int timerCounter = 0;
	std::string id = "timer_" + std::to_string(timerCounter++);

	Timer timer;
	timer.id = id;
	timer.duration = duration;
	timer.elapsed = 0.0f;
	timer.loop = loop;
	timer.active = autoStart;
	timer.onComplete = onComplete;
	timer.onUpdate = onUpdate;

	m_Impl->timers[id] = timer;
	return id;
}

bool CTimeSystem::StartTimer(const std::string& id)
{
	auto it = m_Impl->timers.find(id);
	if (it != m_Impl->timers.end())
	{
		it->second.active = true;
		return true;
	}
	return false;
}

bool CTimeSystem::StopTimer(const std::string& id)
{
	auto it = m_Impl->timers.find(id);
	if (it != m_Impl->timers.end())
	{
		it->second.active = false;
		return true;
	}
	return false;
}

bool CTimeSystem::ResetTimer(const std::string& id)
{
	auto it = m_Impl->timers.find(id);
	if (it != m_Impl->timers.end())
	{
		it->second.elapsed = 0.0f;
		return true;
	}
	return false;
}

bool CTimeSystem::RemoveTimer(const std::string& id)
{
	auto it = m_Impl->timers.find(id);
	if (it != m_Impl->timers.end())
	{
		m_Impl->timers.erase(it);
		return true;
	}
	return false;
}

float CTimeSystem::GetTimerProgress(const std::string& id) const
{
	auto it = m_Impl->timers.find(id);
	if (it != m_Impl->timers.end())
	{
		return it->second.elapsed / it->second.duration;
	}
	return 0.0f;
}

bool CTimeSystem::IsTimerActive(const std::string& id) const
{
	auto it = m_Impl->timers.find(id);
	if (it != m_Impl->timers.end())
	{
		return it->second.active;
	}
	return false;
}

// ========== ВРЕМЕННЫЕ ЭФФЕКТЫ ==========
void CTimeSystem::SetTimeScale(const std::string& id, float scale, float duration)
{
	TimeScale ts;
	ts.id = id;
	ts.scale = scale;
	ts.duration = duration;
	ts.active = true;

	// Заменяем существующий или добавляем новый
	auto it = std::find_if(m_Impl->timeScales.begin(), m_Impl->timeScales.end(),
						   [&id](const TimeScale& ts) { return ts.id == id; });

	if (it != m_Impl->timeScales.end())
	{
		*it = ts;
	}
	else
	{
		m_Impl->timeScales.push_back(ts);
	}
}

float CTimeSystem::GetTimeScale(const std::string& id) const
{
	auto it = std::find_if(m_Impl->timeScales.begin(), m_Impl->timeScales.end(),
						   [&id](const TimeScale& ts) { return ts.id == id; });

	if (it != m_Impl->timeScales.end())
	{
		return it->scale;
	}
	return 1.0f;
}

void CTimeSystem::RemoveTimeScale(const std::string& id)
{
	m_Impl->timeScales.erase(std::remove_if(m_Impl->timeScales.begin(), m_Impl->timeScales.end(),
											[&id](const TimeScale& ts) { return ts.id == id; }),
							 m_Impl->timeScales.end());
}

// ========== МЕТОДЫ ДЛЯ ЭФФЕКТОВ КАМЕРЫ ==========
float CTimeSystem::GetSmoothDeltaTime() const
{
	return m_Impl->deltaTime;
}

float CTimeSystem::GetFixedDeltaTime() const
{
	return m_Impl->fixedDeltaTime;
}

float CTimeSystem::GetUnscaledDeltaTime() const
{
	return m_Impl->unscaledDeltaTime;
}

// ========== ОСЦИЛЛЯТОРЫ ==========
float CTimeSystem::GetSineWave(float frequency, float amplitude, float phase) const
{
	float time = GetTotalTime();
	return sinf(2.0f * 3.1415926535f * frequency * time + phase) * amplitude;
}

float CTimeSystem::GetCosineWave(float frequency, float amplitude, float phase) const
{
	float time = GetTotalTime();
	return cosf(2.0f * 3.1415926535f * frequency * time + phase) * amplitude;
}

float CTimeSystem::GetTriangleWave(float frequency, float amplitude) const
{
	float time = GetTotalTime();
	float t = fmod(time * frequency, 1.0f);
	return (t < 0.5f ? 4.0f * t - 1.0f : 3.0f - 4.0f * t) * amplitude;
}

float CTimeSystem::GetSawtoothWave(float frequency, float amplitude) const
{
	float time = GetTotalTime();
	return (2.0f * fmod(time * frequency, 1.0f) - 1.0f) * amplitude;
}

// ========== ШУМ ==========
float CTimeSystem::GetPerlinNoise2D(float x, float y) const
{
	return m_Impl->perlinNoise.noise(x, y, 0.0f);
}

float CTimeSystem::GetPerlinNoise3D(float x, float y, float z) const
{
	return m_Impl->perlinNoise.noise(x, y, z);
}

float CTimeSystem::GetValueNoise2D(float x, float y) const
{
	return m_Impl->randomDist(m_Impl->randomEngine);
}

// ========== ЦИКЛИЧЕСКОЕ ВРЕМЯ ==========
float CTimeSystem::GetCyclicTime(float cycleDuration) const
{
	if (cycleDuration <= 0.0f)
		return 0.0f;
	return fmod(GetTotalTime(), cycleDuration);
}

float CTimeSystem::GetPingPongTime(float duration) const
{
	if (duration <= 0.0f)
		return 0.0f;
	float t = fmod(GetTotalTime(), duration * 2.0f);
	return (t < duration) ? t : duration * 2.0f - t;
}

// ========== УТИЛИТЫ ДЛЯ ИНТЕРПОЛЯЦИИ ==========
float CTimeSystem::GetLerpFactor(float smoothTime, float deltaTime) const
{
	if (smoothTime <= 0.0f)
		return 1.0f;
	return 1.0f - expf(-deltaTime / smoothTime);
}

float CTimeSystem::GetDampedSpring(float current, float target, float& velocity, float smoothTime,
								   float deltaTime) const
{
	// Реализация пружинного демпфера
	float omega = 2.0f / smoothTime;
	float x = omega * deltaTime;
	float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

	float change = current - target;
	float temp = (velocity + omega * change) * deltaTime;
	velocity = (velocity - omega * temp) * exp;
	float result = target + (change + temp) * exp;

	// Предотвращение превышения цели
	if ((target - current > 0.0f) == (result > target))
	{
		result = target;
		velocity = 0.0f;
	}

	return result;
}

// ========== СТАТИСТИКА ==========
CTimeSystem::PerformanceStats CTimeSystem::GetPerformanceStats() const
{
	PerformanceStats stats = {};

	if (!m_Impl->frameTimes.empty())
	{
		std::vector<float> sortedTimes = m_Impl->frameTimes;
		std::sort(sortedTimes.begin(), sortedTimes.end());

		int indices[5] = {(int)(sortedTimes.size() * 0.5f), (int)(sortedTimes.size() * 0.75f),
						  (int)(sortedTimes.size() * 0.9f), (int)(sortedTimes.size() * 0.95f),
						  (int)(sortedTimes.size() * 0.99f)};

		for (int i = 0; i < 5; i++)
		{
			if (indices[i] < sortedTimes.size())
			{
				stats.frameTimePercentiles[i] = sortedTimes[indices[i]];
			}
		}
	}

	return stats;
}

void CTimeSystem::ResetPerformanceStats()
{
	m_Impl->frameTimes.clear();
	m_Impl->minFPS = 1000.0f;
	m_Impl->maxFPS = 0.0f;
	m_Impl->averageFPS = 0.0f;
}

void CTimeSystem::EnableProfiling(bool enable)
{
	m_Impl->profilingEnabled = enable;
}

bool CTimeSystem::IsProfilingEnabled() const
{
	return m_Impl->profilingEnabled;
}
///////////////////////////////////////////////////////////////
