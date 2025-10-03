///////////////////////////////////////////////////////////////
// Created: 29.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <limits>
#include "math_types.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
template <typename T> struct Constants;

template <> struct Constants<float>
{
	static constexpr float Pi = 3.14159265358979323846f;
	static constexpr float TwoPi = 6.28318530717958647692f;
	static constexpr float HalfPi = 1.57079632679489661923f;
	static constexpr float InvPi = 0.31830988618379067154f;
	static constexpr float InvTwoPi = 0.15915494309189533577f;
	static constexpr float DegToRad = Pi / 180.0f;
	static constexpr float RadToDeg = 180.0f / Pi;
	static constexpr float Epsilon = 1e-6f;
	static constexpr float Infinity = std::numeric_limits<float>::infinity();
};

template <> struct Constants<double>
{
	static constexpr double Pi = 3.14159265358979323846;
	static constexpr double TwoPi = 6.28318530717958647692;
	static constexpr double HalfPi = 1.57079632679489661923;
	static constexpr double InvPi = 0.31830988618379067154;
	static constexpr double InvTwoPi = 0.15915494309189533577;
	static constexpr double DegToRad = Pi / 180.0;
	static constexpr double RadToDeg = 180.0 / Pi;
	static constexpr double Epsilon = 1e-9;
	static constexpr double Infinity = std::numeric_limits<double>::infinity();
};

// ѕсевдонимы дл€ удобства
#define PI Constants<float>::Pi
#define TWO_PI Constants<float>::TwoPi
#define HALF_PI Constants<float>::HalfPi
#define DEG_TO_RAD Constants<float>::DegToRad
#define RAD_TO_DEG Constants<float>::RadToDeg
#define EPSILON Constants<float>::Epsilon

#ifdef INFINITY
#undef INFINITY
#define INFINITY Constants<float>::Infinity
#endif
} // namespace Core::Math
///////////////////////////////////////////////////////////////