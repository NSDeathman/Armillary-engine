///////////////////////////////////////////////////////////////
// Created: 13.02.2025
// Author: NS_Deathman
// Object stage shader pipeline
///////////////////////////////////////////////////////////////
#include "common.ash"
#include "brdf.ash"
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
    float2 UV : TEXCOORD1;
    float3 Normal : TEXCOORD2;
};
///////////////////////////////////////////////////////////////
Interpolators VSMain(VertexData Input)
{
    Interpolators Output;

    Output.HomogeniousPosition = mul(float4(Input.Position, 1.0f), matWorldViewProjectionTransposed);
    Output.Position = mul(float4(Input.Position, 1.0f), matWorldViewTransposed);
    Output.Normal = normalize(mul(Input.Normal, (float3x3) matWorld));
    Output.UV = Input.UV;

    return Output;
}

float4 PSMain(Interpolators Input) : COLOR0
{
    float2 UV = float2(Input.UV.x, 1.0f - Input.UV.y);
    
    float3 Albedo = tex2D(texAlbedo, UV);
    Albedo = sRgbToLinear(Albedo);
    float AO = tex2D(texAO, UV);
    
    float Roughness = tex2D(texRoughness, UV);
    float Metallic = tex2D(texMetallic, UV);
    
    float3 NormalMap = tex2D(texNormal, UV);
    
    NormalMap = NormalMap * 2.0f - 1.0f;
    
    float3x3 TBN = CalculateTBN(Input.Position, Input.Normal, Input.UV);
    
    float3 Normal = mul(TBN, NormalMap);
    
    Normal = normalize(Normal);
        
    float3 LightPosition = float3(1.0f, 5.0f, 10.0f);
    float3 LightColor = float3(1.0f, 0.9f, 1.0f) * 8;
    float3 AmbientBrightness = 0.25f;
    float3 LightDirection = normalize(Input.Position - LightPosition);
    LightComponents Light = Calculate_Lighting_Model(Roughness, Metallic, Albedo, Input.Position, Normal, LightDirection);
    float AttenuationFactor = CalculateAttenuation(Input.Position, LightPosition, -100.0f);
    
    float SelfShading = 1.0f; //safe_dot(LightDirection, Input.Normal);
    float3 DirectLighting = (Light.Diffuse + Light.Specular) * LightColor * SelfShading * AttenuationFactor;
    float3 IndirectLighting = AmbientBrightness * AO;
    float3 FinalColor = (IndirectLighting + DirectLighting) * Albedo;
    //FinalColor = LumaWeightedReinhard(FinalColor);
    FinalColor = LinearTosRgb(FinalColor);
    return FinalColor.xyzz;
}
///////////////////////////////////////////////////////////////
