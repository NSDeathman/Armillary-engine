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
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD0;
};

struct Interpolators
{
    float4 HomogeniousPosition : POSITION;
    float3 Position : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float3 TBN0 : TEXCOORD2;
    float3 TBN1 : TEXCOORD3;
    float3 TBN2 : TEXCOORD4;
    float3 Dir : TEXCOORD5;
};
///////////////////////////////////////////////////////////////
Interpolators VSMain(VertexData Input)
{
    Interpolators Output;
    
    Output.HomogeniousPosition = mul(transpose(matWorldViewProjection), float4(Input.Position, 1.0f));
    Output.Position = mul(transpose(matView), float4(Input.Position, 1.0f));
    
    float3 Tangent = mul((float3x3) transpose(matWorldView), Input.Tangent);
    float3 Normal = mul((float3x3) transpose(matWorldView), Input.Normal);
    float3 Binormal = cross(Normal, Tangent);
    float3x3 TBN = float3x3(Tangent, Binormal, Normal);
    
    Output.TBN0 = TBN[0];
    Output.TBN1 = TBN[1];
    Output.TBN2 = TBN[2];
    
    Output.UV = float2(Input.UV.x, 1.0f - Input.UV.y);

    Output.Dir.xyz = mul((float3x3) transpose(matView), float3(0.2, 0.8, 0.5));
    
    return Output;
}

float4 PSMain(Interpolators Input) : COLOR0
{    
    float3 Albedo = tex2D(texAlbedo, Input.UV);
    Albedo = sRgbToLinear(Albedo);
    float AO = tex2D(texAO, Input.UV);
    
    float Roughness = tex2D(texRoughness, Input.UV);
    float Metallic = tex2D(texMetallic, Input.UV);
    
    float3 NormalMap = tex2D(texNormal, Input.UV);
    
    NormalMap = NormalMap * 2.0f - 1.0f;
    
    float3x3 TBN = float3x3(Input.TBN0, Input.TBN1, Input.TBN2);
    float3 Normal = normalize(mul(NormalMap, TBN));
    float3 FlatNormal = normalize(mul(float3(0, 0, 1), TBN));
        
    float3 LightColor = float3(1.0f, 1.0f, 1.0f);
    float3 LightDirection = Input.Dir;//normalize(LightPosition - Input.Position);
    float AmbientBrightness = 0.1f;
    float SphericalHarmonicsApproximation = saturate(max(FlatNormal.y, 0.0f) + 0.5f);
    
    LightComponents Light = Calculate_Lighting_Model(Roughness, Metallic, Albedo, Input.Position, Normal, LightDirection);
    
    float SelfShadowing = safe_dot(FlatNormal, LightDirection);
    float3 DirectLighting = (Light.Diffuse + Light.Specular) * SelfShadowing * LightColor;
    float3 IndirectLighting = AmbientBrightness * AO * SphericalHarmonicsApproximation * Albedo;
    float3 FinalColor = (IndirectLighting + DirectLighting);
    FinalColor = LumaWeightedReinhard(FinalColor);
    FinalColor = OETF_REC709(FinalColor);
    return FinalColor.xyzz;
}
///////////////////////////////////////////////////////////////
