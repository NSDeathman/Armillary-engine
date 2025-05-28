////////////////////////////////////////////////////////////////////////////////
// Created: 14.01.2025
// Author: OpenXRay, ChatGPT, NS_Deathman
// CPU Identification Code
////////////////////////////////////////////////////////////////////////////////
#pragma once
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
} // namespace CPU
////////////////////////////////////////////////////////////////////////////////
extern void initializeCPU();
////////////////////////////////////////////////////////////////////////////////
