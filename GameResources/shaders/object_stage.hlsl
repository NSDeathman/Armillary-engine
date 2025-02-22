///////////////////////////////////////////////////////////////
// Created: 13.02.2025
// Author: NS_Deathman
// Object stage shader pipeline
///////////////////////////////////////////////////////////////
#include "common.ash"
///////////////////////////////////////////////////////////////
struct VertexData
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
};

struct Interpolators
{
    float4 HomogeniousPosition : POSITION;
    float3 Position : TEXCOORD0;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD1;
};
///////////////////////////////////////////////////////////////
Interpolators VSMain(VertexData Input)
{
    Interpolators Output;

    Output.HomogeniousPosition = mul(float4(Input.Position, 1.0f), matWorldViewProjectionTransposed);
    Output.Position = mul(float4(Input.Position, 1.0f), matViewProjectionTransposed);
    Output.UV = Input.UV;
    Output.Normal = Input.Normal;

    return Output;
}

float4 PSMain(Interpolators Input) : COLOR0
{
    return tex2D(texAlbedo, float2(Input.UV.x, 1.0f - Input.UV.y));
}
///////////////////////////////////////////////////////////////
