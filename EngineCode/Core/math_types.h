///////////////////////////////////////////////////////////////
// Created: 29.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "math_half.h"
#include "math_half2.h"
#include "math_half3.h"
#include "math_half4.h"
#include "math_float2.h"
#include "math_float3.h"
#include "math_float4.h"
#include "math_float3x3.h"
#include "math_float4x4.h"
#include "math_quaternion.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
	using s8 = signed char;
	using u8 = unsigned char;

	using s16 = signed short;
	using u16 = unsigned short;

	using s32 = signed int;
	using u32 = unsigned int;

	using s64 = signed __int64;
	using u64 = unsigned __int64;

	using Byte = uint8_t;
    using Word = uint16_t;
    using DWord = uint32_t;

	using string = std::string;

#ifdef MATH_USE_DOUBLE_PRECISION
	using Real = double;
#else
	using Real = float;
#endif
} // namespace Core::Math
///////////////////////////////////////////////////////////////