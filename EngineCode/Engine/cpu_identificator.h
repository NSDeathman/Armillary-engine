#pragma once
#include "stdafx.h"

enum class CpuFeature : u32
{
	Mmx = 0x0001,
	Sse = 0x0002,
	Sse2 = 0x0004,
	_3dNow = 0x0008,

	Sse3 = 0x0010,
	Ssse3 = 0x0020,
	Sse41 = 0x0040,
	Sse42 = 0x0080,

	MWait = 0x1000,
	HT = 0x0200
};

struct processor_info
{
	string32 vendor;	// vendor name
	string64 modelName; // Name of model eg. Intel_Pentium_Pro

	unsigned char family;	// family of the processor, eg. Intel_Pentium_Pro is family 6 processor
	unsigned char model;	// model of processor, eg. Intel_Pentium_Pro is model 1 of family 6 processor
	unsigned char stepping; // Processor revision number

	unsigned int features; // processor Feature ( same as return value).

	unsigned int n_cores;	// number of available physical cores
	unsigned int n_threads; // number of available logical threads

	unsigned int affinity_mask; // recommended affinity mask
	// all processors available to process
	// except 2nd (and upper) logical threads
	// of the same physical core

	bool hasFeature(const CpuFeature feature)
	{
		return (features & static_cast<u32>(feature)) != 0;
	}
};

unsigned int query_processor_info(processor_info*);

namespace CPU
{
	extern u64 clk_per_second;
	extern u64 clk_per_milisec;
	extern u64 clk_per_microsec;
	extern u64 clk_overhead;
	extern float clk_to_seconds;
	extern float clk_to_milisec;
	extern float clk_to_microsec;

	extern u64 qpc_freq;
	extern u64 qpc_overhead;
	extern u32 qpc_counter;

	extern processor_info ID;
	extern u64 QPC();

	#ifndef _M_AMD64
	#pragma warning(disable : 4035)
	u64 GetCLK(void)
	{
		_asm _emit 0x0F;
		_asm _emit 0x31;
	}
	#pragma warning(default : 4035)
	#else
	u64 GetCLK(void)
	{
		return __rdtsc();
	}
	#endif
}; // namespace CPU

extern void _initialize_cpu();