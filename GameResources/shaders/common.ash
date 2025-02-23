///////////////////////////////////////////////////////////////
// Created: 13.02.2025
// Author: NS_Deathman
// Object stage shader pipeline
///////////////////////////////////////////////////////////////
#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED
///////////////////////////////////////////////////////////////
#define NULL 0.0f
#define EXP 2.7182818284f
#define PI 3.14159265358979323846f
#define INV_PI 1.0f / PI
#define INV_TWO_PI PI * 0.5
///////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////
// Inverting
#define invert(x) (1.0f - (x))
///////////////////////////////////////////////////////////////
// Myltiply add
#define mad(m, a, b) ((m) * (a) + (b))

// Multiply + Saturation
#define mul_sat(a, b) saturate(mul((a), (b)))

// Multiply add + Saturation
#define mad_sat(m, a, b) saturate(mad(m, a, b))

//Power
#define pow2(x) mul((x), (x))
#define pow4(x) pow2(mul((x), (x)))
#define pow5(x) pow((x), 5.0f)
#define pow6(x) pow((x), 6.0f)
#define powEXP(x) pow((x), 2.718f)
///////////////////////////////////////////////////////////////
//https://www.xrhadertoy.com/view/3tlBW7
#define safe_dot(a, b) max(dot((a), (b)), 0.00001f)
///////////////////////////////////////////////////////////////
uniform float4x4 matView : register(c0);
uniform float4x4 matProjection : register(c4);
uniform float4x4 matWorld : register(c9);
uniform float4x4 matWorldViewProjectionTransposed : register(c13);
uniform float4x4 matViewProjectionTransposed : register(c17);
uniform float4x4 matWorldViewTransposed : register(c21);
///////////////////////////////////////////////////////////////
uniform sampler2D texAlbedo : register(s0);
uniform sampler2D texNormal : register(s1);
uniform sampler2D texRoughness : register(s2);
uniform sampler2D texMetallic : register(s3);
uniform sampler2D texAO : register(s4);
///////////////////////////////////////////////////////////////
float3x3 CalculateTBN(float3 Position, float3 Normal, float2 UV)
{
    // Get edge vectors of the pixel triangle
    float3 dp1 = ddx(Position.xyz);
    float3 dp2 = ddy(Position.xyz);
	
    float2 duv1 = ddx(UV.xy);
    float2 duv2 = ddy(UV.xy);

    float3 N = normalize(Normal);

    // Solve the linear system
    float3 dp2perp = cross(dp2, N);
    float3 dp1perp = cross(N, dp1);
	
    float3 T = normalize(dp2perp * duv1.x + dp1perp * duv2.x);
    float3 B = normalize(dp2perp * duv1.y + dp1perp * duv2.y);
	
    return float3x3( T.x, B.x, N.x,
                     T.y, B.y, N.y,
                     T.z, B.z, N.z );
}
///////////////////////////////////////////////////////////////
float3 sRgbToLinear(float3 vValue)
{
    return vValue * (vValue * (vValue * 0.305306011 + 0.682171111) + 0.012522878);
}

float3 LinearTosRgb(float3 vColor)
{
    float3 S1 = sqrt(vColor);
    float3 S2 = sqrt(S1);
    float3 S3 = sqrt(S2);
    return (0.585122381 * S1 + 0.783140355 * S2 - 0.368262736 * S3);
}
///////////////////////////////////////////////////////////////
static const float3 luminance_weights = float3(0.2125f, 0.7154f, 0.0721f);
///////////////////////////////////////////////////////////////
float3 LumaWeightedReinhard(float3 Color)
{
    const float white_level = 1.0f;
    const float luminance_saturation = 1.00f;
    const float pixel_luminance = dot(Color, luminance_weights);
    const float tone_mapped_luminance = 1.0f - exp(-pixel_luminance / white_level);
    return tone_mapped_luminance * pow(Color / pixel_luminance, luminance_saturation) * white_level;
}
///////////////////////////////////////////////////////////////
#endif//COMMON_H_INCLUDED
///////////////////////////////////////////////////////////////
