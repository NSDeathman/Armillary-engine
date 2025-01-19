#include "cpu_identificator.h"
#include <array>
#include <bitset>
#include <memory>
#include <intrin.h>
#include "log.h"

void nativeCpuId(int regs[4], int i)
{
	__cpuid((int*)regs, (int)i);
}

DWORD countSetBits(ULONG_PTR bitMask)
{
	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = static_cast<ULONG_PTR>(1) << LSHIFT;
	DWORD i;

	for (i = 0; i <= LSHIFT; ++i)
	{
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest /= 2;
	}

	return bitSetCount;
}

unsigned int query_processor_info(processor_info* pinfo)
{
	ZeroMemory(pinfo, sizeof(processor_info));

	std::bitset<32> f_1_ECX;
	std::bitset<32> f_1_EDX;
	std::bitset<32> f_81_EDX;

	std::vector<std::array<int, 4>> data;
	std::array<int, 4> cpui;

	nativeCpuId(cpui.data(), 0);
	const int nIds = cpui[0];

	for (int i = 0; i <= nIds; ++i)
	{
		__cpuidex(cpui.data(), i, 0);
		data.push_back(cpui);
	}

	memset(pinfo->vendor, 0, sizeof(pinfo->vendor));
	*reinterpret_cast<int*>(pinfo->vendor) = data[0][1];
	*reinterpret_cast<int*>(pinfo->vendor + 4) = data[0][3];
	*reinterpret_cast<int*>(pinfo->vendor + 8) = data[0][2];

	// const bool isIntel = std::strncmp(pinfo->vendor, "GenuineIntel", 12);
	const bool isAmd = strncmp(pinfo->vendor, "AuthenticAMD", 12) != 0;

	// load bitset with flags for function 0x00000001
	if (nIds >= 1)
	{
		f_1_ECX = data[1][2];
		f_1_EDX = data[1][3];
	}

	nativeCpuId(cpui.data(), 0x80000000);
	const int nExIds_ = cpui[0];
	data.clear();

	for (int i = 0x80000000; i <= nExIds_; ++i)
	{
		__cpuidex(cpui.data(), i, 0);
		data.push_back(cpui);
	}

	// load bitset with flags for function 0x80000001
	if (nExIds_ >= 0x80000001)
	{
		// f_81_ECX = data[1][2];
		f_81_EDX = data[1][3];
	}

	memset(pinfo->modelName, 0, sizeof(pinfo->modelName));

	// Interpret CPU brand string if reported
	if (nExIds_ >= 0x80000004)
	{
		memcpy(pinfo->modelName, data[2].data(), sizeof(cpui));
		memcpy(pinfo->modelName + 16, data[3].data(), sizeof(cpui));
		memcpy(pinfo->modelName + 32, data[4].data(), sizeof(cpui));
	}

	if (f_1_EDX[23])
		pinfo->features |= static_cast<u32>(CpuFeature::Mmx);
	if (f_1_EDX[25])
		pinfo->features |= static_cast<u32>(CpuFeature::Sse);
	if (f_1_EDX[26])
		pinfo->features |= static_cast<u32>(CpuFeature::Sse2);
	if (isAmd && f_81_EDX[31])
		pinfo->features |= static_cast<u32>(CpuFeature::_3dNow);

	if (f_1_ECX[0])
		pinfo->features |= static_cast<u32>(CpuFeature::Sse3);
	if (f_1_ECX[9])
		pinfo->features |= static_cast<u32>(CpuFeature::Ssse3);
	if (f_1_ECX[19])
		pinfo->features |= static_cast<u32>(CpuFeature::Sse41);
	if (f_1_ECX[20])
		pinfo->features |= static_cast<u32>(CpuFeature::Sse42);

	nativeCpuId(cpui.data(), 1);

	const bool hasMWait = (cpui[2] & 0x8) > 0;
	if (hasMWait)
		pinfo->features |= static_cast<u32>(CpuFeature::MWait);

	pinfo->family = (cpui[0] >> 8) & 0xf;
	pinfo->model = (cpui[0] >> 4) & 0xf;
	pinfo->stepping = cpui[0] & 0xf;

	// Calculate available processors
	ULONG_PTR pa_mask_save, sa_mask_stub = 0;
	GetProcessAffinityMask(GetCurrentProcess(), &pa_mask_save, &sa_mask_stub);

	DWORD returnedLength = 0;
	DWORD byteOffset = 0;
	GetLogicalProcessorInformation(nullptr, &returnedLength);

	auto buffer = std::make_unique<u8[]>(returnedLength);
	auto ptr = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(buffer.get());
	GetLogicalProcessorInformation(ptr, &returnedLength);

	auto processorCoreCount = 0u;
	auto logicalProcessorCount = 0u;

	while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnedLength)
	{
		switch (ptr->Relationship)
		{
		case RelationProcessorCore:
			processorCoreCount++;

			// A hyperthreaded core supplies more than one logical processor.
			logicalProcessorCount += countSetBits(ptr->ProcessorMask);
			break;

		default:
			break;
		}

		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}

	if (logicalProcessorCount != processorCoreCount)
		pinfo->features |= static_cast<u32>(CpuFeature::HT);

	// All logical processors
	pinfo->n_threads = logicalProcessorCount;
	pinfo->affinity_mask = pa_mask_save;
	pinfo->n_cores = processorCoreCount;

	return pinfo->features;
}

namespace CPU
{
	u64 clk_per_second;
	u64 clk_per_milisec;
	u64 clk_per_microsec;
	u64 clk_overhead;
	float clk_to_seconds;
	float clk_to_milisec;
	float clk_to_microsec;
	u64 qpc_freq = 0;
	u64 qpc_overhead = 0;
	u32 qpc_counter = 0;

	processor_info ID;

	u64 QPC()
	{
		u64 _dest;
		QueryPerformanceCounter((PLARGE_INTEGER)&_dest);
		qpc_counter++;
		return _dest;
	}

	void Detect()
	{
		// Timers & frequency
		u64 start, end;
		u32 dwStart, dwTest;

		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

		// Detect Freq
		dwTest = timeGetTime();
		do
		{
			dwStart = timeGetTime();
		} while (dwTest == dwStart);
		start = GetCLK();
		while (timeGetTime() - dwStart < 1000)
			;
		end = GetCLK();
		clk_per_second = end - start;

		// Detect RDTSC Overhead
		clk_overhead = 0;
		u64 dummy = 0;
		int i = 0;
		for (; i < 256; i++)
		{
			start = GetCLK();
			clk_overhead += GetCLK() - start - dummy;
		}
		clk_overhead /= 256;

		// Detect QPC Overhead
		QueryPerformanceFrequency((PLARGE_INTEGER)&qpc_freq);
		qpc_overhead = 0;
		for (i = 0; i < 256; i++)
		{
			start = QPC();
			qpc_overhead += QPC() - start - dummy;
		}
		qpc_overhead /= 256;

		SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

		clk_per_second -= clk_overhead;
		clk_per_milisec = clk_per_second / 1000;
		clk_per_microsec = clk_per_milisec / 1000;

#ifndef WIN64
		_control87(_PC_64, MCW_PC);
#endif
		clk_to_seconds = float(double(1.0 / double(clk_per_second)));
		clk_to_milisec = float(double(1000.0 / double(clk_per_second)));
		clk_to_microsec = float(double(1000000.0 / double(clk_per_second)));
	}
}; // namespace CPU

//------------------------------------------------------------------------------------
void _initialize_cpu(void)
{
	CPU::Detect();

	// General CPU identification
	if (!query_processor_info(&CPU::ID))
		ERROR_MESSAGE("! Can't detect CPU/FPU");

	Msg("* CPU Info:");
	Msg("* CPU Frequency: %.2f mhz", float(CPU::clk_per_second / u64(1000000)));
	Msg("* CPU Thread count: %d", std::thread::hardware_concurrency());

	string256 features;
	strcpy(features, "RDTSC");
	if (CPU::ID.hasFeature(CpuFeature::Sse))
		strcat(features, ", SSE");
	if (CPU::ID.hasFeature(CpuFeature::Sse2))
		strcat(features, ", SSE2");
	if (CPU::ID.hasFeature(CpuFeature::Sse3))
		strcat(features, ", SSE3");
	if (CPU::ID.hasFeature(CpuFeature::Sse41))
		strcat(features, ", SSE4.1");
	if (CPU::ID.hasFeature(CpuFeature::Sse42))
		strcat(features, ", SSE4.2");
	if (CPU::ID.hasFeature(CpuFeature::Ssse3))
		strcat(features, ", SSSE3");
	if (CPU::ID.hasFeature(CpuFeature::Mmx))
		strcat(features, ", MMX");
	if (CPU::ID.hasFeature(CpuFeature::_3dNow))
		strcat(features, ", 3DNow!");
	if (CPU::ID.hasFeature(CpuFeature::MWait))
		strcat(features, ", MONITOR/MWAIT");
	if (CPU::ID.hasFeature(CpuFeature::HT))
		strcat(features, ", HTT");

	Msg("* CPU features: %s \n", features);
}
