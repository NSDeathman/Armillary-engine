///////////////////////////////////////////////////////////////
// Created: 29.09.2025
// Author: NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "math_types.h"
#include "math_constants.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
// ÷ветовые пространства
inline float3 RGBToHSV(const float3& rgb)
{
	float3 hsv;
	Real minVal = fmin(fmin(rgb.x, rgb.y), rgb.z);
	Real maxVal = fmax(fmax(rgb.x, rgb.y), rgb.z);
	Real delta = maxVal - minVal;

	hsv.z = maxVal;

	if (delta < EPSILON)
	{
		hsv.x = 0;
		hsv.y = 0;
	}
	else
	{
		hsv.y = delta / maxVal;

		if (rgb.x >= maxVal)
			hsv.x = (rgb.y - rgb.z) / delta;
		else if (rgb.y >= maxVal)
			hsv.x = 2 + (rgb.z - rgb.x) / delta;
		else
			hsv.x = 4 + (rgb.x - rgb.y) / delta;

		hsv.x *= 60;
		if (hsv.x < 0)
			hsv.x += 360;
	}

	return hsv;
}

inline float3 HSVToRGB(const float3& hsv)
{
	float3 rgb;

	if (hsv.y <= EPSILON)
	{
		rgb = float3(hsv.z);
	}
	else
	{
		Real hh = hsv.x;
		if (hh >= 360)
			hh = 0;
		hh /= 60;

		int i = static_cast<int>(hh);
		Real ff = hh - i;
		Real p = hsv.z * (1 - hsv.y);
		Real q = hsv.z * (1 - (hsv.y * ff));
		Real t = hsv.z * (1 - (hsv.y * (1 - ff)));

		switch (i)
		{
		case 0:
			rgb = float3(hsv.z, t, p);
			break;
		case 1:
			rgb = float3(q, hsv.z, p);
			break;
		case 2:
			rgb = float3(p, hsv.z, t);
			break;
		case 3:
			rgb = float3(p, q, hsv.z);
			break;
		case 4:
			rgb = float3(t, p, hsv.z);
			break;
		case 5:
		default:
			rgb = float3(hsv.z, p, q);
			break;
		}
	}

	return rgb;
}

// Gamma correction
inline float3 LinearToSRGB(const float3& linear)
{
	float3 srgb;
	srgb.x = (linear.x <= 0.0031308f) ? 12.92f * linear.x : 1.055f * powf(linear.x, 1.0f / 2.4f) - 0.055f;
	srgb.y = (linear.y <= 0.0031308f) ? 12.92f * linear.y : 1.055f * powf(linear.y, 1.0f / 2.4f) - 0.055f;
	srgb.z = (linear.z <= 0.0031308f) ? 12.92f * linear.z : 1.055f * powf(linear.z, 1.0f / 2.4f) - 0.055f;
	return srgb;
}

inline float3 SRGBToLinear(const float3& srgb)
{
	float3 linear;
	linear.x = (srgb.x <= 0.04045f) ? srgb.x / 12.92f : powf((srgb.x + 0.055f) / 1.055f, 2.4f);
	linear.y = (srgb.y <= 0.04045f) ? srgb.y / 12.92f : powf((srgb.y + 0.055f) / 1.055f, 2.4f);
	linear.z = (srgb.z <= 0.04045f) ? srgb.z / 12.92f : powf((srgb.z + 0.055f) / 1.055f, 2.4f);
	return linear;
}
} // namespace Core::Math
///////////////////////////////////////////////////////////////
