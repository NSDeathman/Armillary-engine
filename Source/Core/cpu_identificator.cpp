////////////////////////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: DeepSeek, OpenXRay, ChatGPT, NS_Deathman
// CPU Identification Code
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <bitset>
#include <cstring>
#include <array>
#include <vector>
#include <memory>
#include <windows.h>
#include <thread>
#include <iostream>
////////////////////////////////////////////////////////////////////////////////
#include "cpu_identificator.h"
#include "log.h"
////////////////////////////////////////////////////////////////////////////////
namespace Core
{
	namespace CPU
	{
		enum class Feature : uint32_t
		{
			MMX = 0x0001,
			SSE = 0x0002,
			SSE2 = 0x0004,
			SSE3 = 0x0010,
			SSSE3 = 0x0020,
			SSE41 = 0x0040,
			SSE42 = 0x0080,
			AVX = 0x0100,
			AVX2 = 0x0200,
			AVX512 = 0x0400,
		};

		struct ProcessorInfo
		{
			char vendor[12] = {};
			char modelName[64] = {};
			uint8_t family = 0;
			uint8_t model = 0;
			uint8_t stepping = 0;
			uint32_t features = 0;
			uint32_t coreCount = 0;
			uint32_t threadCount = 0;

			bool HasFeature(Feature feature) const
			{
				return (features & static_cast<uint32_t>(feature)) != 0;
			}
		};

		uint64_t clockFrequency = 0;
		uint64_t clockOverhead = 0;
		uint64_t qpcFrequency = 0;
		uint64_t qpcOverhead = 0;

		ProcessorInfo Info;

		void ExecuteCPUID(int registers[4], int functionId)
		{
			__cpuid(registers, functionId);
		}

		uint32_t DetectFeatures(ProcessorInfo* info)
		{
			int registers[4];

			// Get vendor std::string and maximum function ID
			ExecuteCPUID(registers, 0);
			const int maxFunctionId = registers[0];

			// Правильное копирование vendor std::string (12 байт из трех регистров)
			memcpy(info->vendor, &registers[1], 4);		// EBX
			memcpy(info->vendor + 4, &registers[2], 4); // EDX
			memcpy(info->vendor + 8, &registers[3], 4); // ECX
			info->vendor[12] = '\0';					// Vendor std::string всегда 12 символов

			// Get processor info and basic features
			if (maxFunctionId >= 1)
			{
				ExecuteCPUID(registers, 1);

				// Extract processor family, model, stepping
				info->stepping = registers[0] & 0xF;
				info->model = (registers[0] >> 4) & 0xF;
				info->family = (registers[0] >> 8) & 0xF;

				// Check feature flags
				const uint32_t ecx = registers[2];
				const uint32_t edx = registers[3];

				info->features |= (edx & (1 << 23)) ? (uint32_t)Feature::MMX : 0;
				info->features |= (edx & (1 << 25)) ? (uint32_t)Feature::SSE : 0;
				info->features |= (edx & (1 << 26)) ? (uint32_t)Feature::SSE2 : 0;
				info->features |= (ecx & (1 << 0)) ? (uint32_t)Feature::SSE3 : 0;
				info->features |= (ecx & (1 << 9)) ? (uint32_t)Feature::SSSE3 : 0;
				info->features |= (ecx & (1 << 19)) ? (uint32_t)Feature::SSE41 : 0;
				info->features |= (ecx & (1 << 20)) ? (uint32_t)Feature::SSE42 : 0;
			}

			// Check for extended features (AVX, AVX2, AVX512)
			if (maxFunctionId >= 7)
			{
				ExecuteCPUID(registers, 7);
				const uint32_t ebx = registers[1];

				info->features |= (ebx & (1 << 5)) ? (uint32_t)Feature::AVX : 0;
				info->features |= (ebx & (1 << 16)) ? (uint32_t)Feature::AVX2 : 0;
				info->features |= (ebx & (1 << 30)) ? (uint32_t)Feature::AVX512 : 0;
			}

			// Get processor name (extended function 0x80000002-0x80000004)
			int extendedMaxFunctionId;
			ExecuteCPUID(registers, 0x80000000);
			extendedMaxFunctionId = registers[0];

			if (extendedMaxFunctionId >= 0x80000004)
			{
				char* namePtr = info->modelName;
				ExecuteCPUID((int*)namePtr, 0x80000002);
				ExecuteCPUID((int*)(namePtr + 16), 0x80000003);
				ExecuteCPUID((int*)(namePtr + 32), 0x80000004);
				info->modelName[48] = '\0'; // 16 байт × 3 = 48 символов
			}

			return info->features;
		}

		void DetectClockFrequency()
		{
			// Elevate priority for accurate measurement
			const HANDLE process = GetCurrentProcess();
			const DWORD originalPriority = GetPriorityClass(process);
			SetPriorityClass(process, REALTIME_PRIORITY_CLASS);

			// Measure RDTSC frequency
			const DWORD startTime = timeGetTime();
			uint64_t startTsc, endTsc;

			// Wait for time change to align measurement
			while (timeGetTime() == startTime)
			{
			}

			startTsc = ReadTimeStampCounter();
			while (timeGetTime() - startTime < 1000)
			{
			} // Wait 1 second
			endTsc = ReadTimeStampCounter();

			clockFrequency = endTsc - startTsc;

			// Measure RDTSC overhead
			for (int i = 0; i < 100; ++i)
			{
				startTsc = ReadTimeStampCounter();
				clockOverhead += ReadTimeStampCounter() - startTsc;
			}
			clockOverhead /= 100;

			// Adjust for overhead
			clockFrequency -= clockOverhead;

			// Measure QPC frequency and overhead
			QueryPerformanceFrequency((LARGE_INTEGER*)&qpcFrequency);

			for (int i = 0; i < 100; ++i)
			{
				uint64_t startQpc;
				QueryPerformanceCounter((LARGE_INTEGER*)&startQpc);
				qpcOverhead += ReadTimeStampCounter() - startTsc;
			}
			qpcOverhead /= 100;

			// Restore original priority
			SetPriorityClass(process, originalPriority);
		}

		void Initialize()
		{
			DetectClockFrequency();
			DetectFeatures(&Info);

			// Get core/thread count from system info
			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);
			Info.coreCount = systemInfo.dwNumberOfProcessors;

			// For thread count, we can use hardware_concurrency or WMI
			Info.threadCount = std::thread::hardware_concurrency();
		}
	} // namespace CPU

	CORE_API void InitializeCPU()
	{
		CPU::Initialize();

		Log("CPU Information:");
		Log("Vendor: %s", CPU::Info.vendor);
		Log("Model: %s", CPU::Info.modelName);
		Log("Cores: %d", CPU::Info.coreCount);
		Log("Threads: %d", CPU::Info.threadCount);
		Log("Frequency: %f %s", ((float)CPU::clockFrequency / 1000000000), "GHz");

		std::string features;
		if (CPU::Info.HasFeature(CPU::Feature::MMX))
			features += "MMX, ";
		if (CPU::Info.HasFeature(CPU::Feature::SSE))
			features += "SSE, ";
		if (CPU::Info.HasFeature(CPU::Feature::SSE2))
			features += "SSE2, ";
		if (CPU::Info.HasFeature(CPU::Feature::SSE3))
			features += "SSE3, ";
		if (CPU::Info.HasFeature(CPU::Feature::SSSE3))
			features += "SSSE3, ";
		if (CPU::Info.HasFeature(CPU::Feature::SSE41))
			features += "SSE4.1, ";
		if (CPU::Info.HasFeature(CPU::Feature::SSE42))
			features += "SSE4.2, ";
		if (CPU::Info.HasFeature(CPU::Feature::AVX))
			features += "AVX, ";
		if (CPU::Info.HasFeature(CPU::Feature::AVX2))
			features += "AVX2, ";
		if (CPU::Info.HasFeature(CPU::Feature::AVX512))
			features += "AVX512, ";

		if (!features.empty())
		{
			features = features.substr(0, features.length() - 2);
			Log("Features: %s \n", features.c_str());
		}
	}
} // namespace Core
////////////////////////////////////////////////////////////////////////////////
