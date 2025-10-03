///////////////////////////////////////////////////////////////
// Created: 29.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <d3dx9.h>
///////////////////////////////////////////////////////////////
#include "math_types.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
// Конвертация float2 -> D3DXVECTOR2
inline D3DXVECTOR2 ToD3DXVECTOR2(const float2& vec)
{
	return D3DXVECTOR2(vec.x, vec.y);
}

// Конвертация float3 -> D3DXVECTOR3
inline D3DXVECTOR3 ToD3DXVECTOR3(const float3& vec)
{
	return D3DXVECTOR3(vec.x, vec.y, vec.z);
}

// Конвертация float4 -> D3DXVECTOR4
inline D3DXVECTOR4 ToD3DXVECTOR4(const float4& vec)
{
	return D3DXVECTOR4(vec.x, vec.y, vec.z, vec.w);
}

// Конвертация из D3DXVECTOR*
inline float2 FromD3DXVECTOR2(const D3DXVECTOR2& vec)
{
	return float2(vec.x, vec.y);
}

inline float3 FromD3DXVECTOR3(const D3DXVECTOR3& vec)
{
	return float3(vec.x, vec.y, vec.z);
}

inline float4 FromD3DXVECTOR4(const D3DXVECTOR4& vec)
{
	return float4(vec.x, vec.y, vec.z, vec.w);
}

// Для D3DCOLOR
inline float4 FromD3DCOLOR(D3DCOLOR color)
{
	return float4(((color >> 16) & 0xFF) / 255.0f, // R
				  ((color >> 8) & 0xFF) / 255.0f,  // G
				  (color & 0xFF) / 255.0f,		   // B
				  ((color >> 24) & 0xFF) / 255.0f  // A
	);
}

inline D3DCOLOR ToD3DCOLOR(const float4& color)
{
	return D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
}
} // namespace Core::Math
///////////////////////////////////////////////////////////////
