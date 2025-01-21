////////////////////////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: OpenXRay, ChatGPT, NS_Deathman
// CPU Identification Code
////////////////////////////////////////////////////////////////////////////////
#include "cpu_identificator.h"
#include "log.h"
////////////////////////////////////////////////////////////////////////////////
namespace CPU
{
enum class Feature : uint32_t
{
	Mmx = 0x0001,
	Sse = 0x0002,
	Sse2 = 0x0004,
	Sse3 = 0x0010,
	Ssse3 = 0x0020,
	Sse41 = 0x0040,
	Sse42 = 0x0080,
	Avx = 0x0100,
	Avx2 = 0x0200,
	Avx512 = 0x0400,
};

struct ProcessorInfo
{
	char vendor[32] = {};
	char modelName[64] = {};
	uint8_t family = 0;
	uint8_t model = 0;
	uint8_t stepping = 0;
	uint32_t features = 0;
	uint32_t n_cores = 0;
	uint32_t n_threads = 0;
	uint32_t affinity_mask = 0;

	bool hasFeature(CPU::Feature feature) const
	{
		return (features & static_cast<uint32_t>(feature)) != 0;
	}
};

uint64_t clk_per_second;
uint64_t clk_per_millisec;
uint64_t clk_per_microsec;
float clk_to_seconds;
float clk_to_milisec;
float clk_to_microsec;
uint64_t clk_overhead;
uint64_t qpc_freq = 0;
uint64_t qpc_overhead = 0;
uint32_t qpc_counter = 0;

ProcessorInfo ID;

void nativeCpuId(int regs[4], int i)
{
	__cpuid(regs, i);
}

DWORD countSetBits(ULONG_PTR bitMask)
{
	DWORD count = 0;
	while (bitMask)
	{
		count += bitMask & 1;
		bitMask >>= 1;
	}
	return count;
}

#ifndef _M_AMD64
#pragma warning(disable : 4035)
uint64_t GetCLK(void)
{
	_asm _emit 0x0F;
	_asm _emit 0x31;
}
#pragma warning(default : 4035)
#else
uint64_t GetCLK(void)
{
	return __rdtsc();
}
#endif

unsigned int queryProcessorInfo(ProcessorInfo* pinfo)
{
	ZeroMemory(pinfo, sizeof(ProcessorInfo));

	int cpui[4];
	nativeCpuId(cpui, 0);
	const int nIds = cpui[0];

	for (int i = 0; i <= nIds; ++i)
	{
		nativeCpuId(cpui, i);
		if (i == 0)
		{
			memcpy(pinfo->vendor, &cpui[1], sizeof(pinfo->vendor) - 1);
		}
		else if (i == 1)
		{
			std::bitset<32> f_ECX(cpui[2]);
			std::bitset<32> f_EDX(cpui[3]);

			// Set features based on flags
			pinfo->features |= f_EDX[23] ? static_cast<uint32_t>(CPU::Feature::Mmx) : 0;
			pinfo->features |= f_EDX[25] ? static_cast<uint32_t>(CPU::Feature::Sse) : 0;
			pinfo->features |= f_EDX[26] ? static_cast<uint32_t>(CPU::Feature::Sse2) : 0;
			pinfo->features |= f_ECX[0] ? static_cast<uint32_t>(CPU::Feature::Sse3) : 0;
			pinfo->features |= f_ECX[9] ? static_cast<uint32_t>(CPU::Feature::Ssse3) : 0;
			pinfo->features |= f_ECX[19] ? static_cast<uint32_t>(CPU::Feature::Sse41) : 0;
			pinfo->features |= f_ECX[20] ? static_cast<uint32_t>(CPU::Feature::Sse42) : 0;
		}
		else if (i == 7) // Check for advanced features
		{
			std::bitset<32> f_EBX(cpui[1]);
			std::bitset<32> f_ECX(cpui[2]);

			pinfo->features |= f_EBX[5] ? static_cast<uint32_t>(CPU::Feature::Avx) : 0;		// AVX
			pinfo->features |= f_EBX[16] ? static_cast<uint32_t>(CPU::Feature::Avx2) : 0;	// AVX2
			pinfo->features |= f_EBX[30] ? static_cast<uint32_t>(CPU::Feature::Avx512) : 0; // AVX512
		}
	}

	return pinfo->features;
}

uint64_t QPC()
{
	uint64_t _dest;
	QueryPerformanceCounter((PLARGE_INTEGER)&_dest);
	qpc_counter++;
	return _dest;
}

void Detect()
{
	// Set process priority to real-time
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	// Detect frequency
	uint64_t startTime, endTime;
	uint32_t initialTime, currentTime;

	// Wait for timeGetTime to change
	initialTime = timeGetTime();
	do
	{
		currentTime = timeGetTime();
	} while (initialTime == currentTime);

	startTime = GetCLK();
	while (timeGetTime() - currentTime < 1000)
		; // Busy wait for 1 second
	endTime = GetCLK();

	clk_per_second = endTime - startTime;

	// Calculate RDTSC overhead
	clk_overhead = 0;
	for (int i = 0; i < 256; ++i)
	{
		startTime = GetCLK();
		clk_overhead += GetCLK() - startTime;
	}
	clk_overhead /= 256;

	// Calculate QPC overhead
	QueryPerformanceFrequency((PLARGE_INTEGER)&qpc_freq);
	qpc_overhead = 0;
	for (int i = 0; i < 256; ++i)
	{
		startTime = QPC();
		qpc_overhead += QPC() - startTime;
	}
	qpc_overhead /= 256;

	// Reset process priority to normal
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

	// Adjust clock counts
	clk_per_second -= clk_overhead;
	clk_per_millisec = clk_per_second / 1000;
	clk_per_microsec = clk_per_millisec / 1000;

#ifndef WIN64
	_control87(_PC_64, MCW_PC); // Ensure precision control for 32-bit builds
#endif

	// Convert clock ticks to time units
	clk_to_seconds = 1.0f / clk_per_second;
	clk_to_milisec = 1000.0f / clk_per_second;
	clk_to_microsec = 1000000.0f / clk_per_second;
}
} // namespace CPU

void initializeCPU()
{
	CPU::Detect();
	if (!queryProcessorInfo(&CPU::ID))
	{
		Msg("Can't detect CPU/FPU");
		return;
	}

	Msg("CPU Info:");
	Msg("CPU Thread count: %d", std::thread::hardware_concurrency());
	Msg("CPU Frequency: %.2f MHZ", float(CPU::clk_per_second / u64(1000000)));

	string256 features;
	strcpy(features, "");
	if (CPU::ID.hasFeature(CPU::Feature::Sse))
		strcat(features, "SSE");
	if (CPU::ID.hasFeature(CPU::Feature::Sse2))
		strcat(features, ", SSE2");
	if (CPU::ID.hasFeature(CPU::Feature::Sse3))
		strcat(features, ", SSE3");
	if (CPU::ID.hasFeature(CPU::Feature::Sse41))
		strcat(features, ", SSE4.1");
	if (CPU::ID.hasFeature(CPU::Feature::Sse42))
		strcat(features, ", SSE4.2");
	if (CPU::ID.hasFeature(CPU::Feature::Ssse3))
		strcat(features, ", SSSE3");
	if (CPU::ID.hasFeature(CPU::Feature::Mmx))
		strcat(features, ", MMX");
	if (CPU::ID.hasFeature(CPU::Feature::Avx))
		strcat(features, ", AVX");
	if (CPU::ID.hasFeature(CPU::Feature::Avx2))
		strcat(features, ", AVX2");
	if (CPU::ID.hasFeature(CPU::Feature::Avx512))
		strcat(features, ", AVX512");

	Msg("CPU features: %s\n", features);
}
////////////////////////////////////////////////////////////////////////////////
