///////////////////////////////////////////////////////////////
// Created: 29.09.2025
// Author: NS_Deathman
///////////////////////////////////////////////////////////////
//  /\_/\
// ( ^.^ )
// /  ~  \ 
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "math_types.h"
#include "math_functions.h"
#include "math_constants.h"
#include "math_color_functions.h"
#include "math_fast_trigonometry.h"
///////////////////////////////////////////////////////////////
// List of things whats we will exports
// from Core.dll to other engine components
///////////////////////////////////////////////////////////////
namespace Core::Math
{
	class half;
	class half2;
	class half3;
	class half4;

	class float2;
	class float3;
	class float4;

	class float3x3;
	class float4x4;

	class quaternion;

	inline float fast_sin(int angle_in_degrees);
	inline float fast_cos(int angle_in_degrees);

	inline float3 RGBToHSV(const float3& rgb);
	inline float3 HSVToRGB(const float3& hsv);

	inline float3 LinearToSRGB(const float3& linear);
	inline float3 SRGBToLinear(const float3& srgb);
} // namespace Core::Math
///////////////////////////////////////////////////////////////
