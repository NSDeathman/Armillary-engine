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
float3 transform_dir(float3 v)
{
    float3 dest = 0.0f;
    dest.x = v.x * matView._11 + v.y * matView._21 + v.z * matView._31;
    dest.y = v.x * matView._12 + v.y * matView._22 + v.z * matView._32;
    dest.z = v.x * matView._13 + v.y * matView._23 + v.z * matView._33;
    return dest;
}

Interpolators VSMain(VertexData Input)
{
    Interpolators Output;
    
    Output.HomogeniousPosition = mul(float4(Input.Position, 1.0f), matWorldViewProjection);
    Output.Position = mul(float4(Input.Position, 1.0f), transpose(matWorldView)).xyz;
    
    float3 Binormal = cross(Input.Normal, Input.Tangent);
    float3x3 TBN = float3x3(Input.Tangent, Binormal, Input.Normal);
    TBN = mul((float3x3) transpose(matWorldView), TBN);
    
    Output.TBN0 = TBN[0];
    Output.TBN1 = TBN[1];
    Output.TBN2 = TBN[2];
    
    Output.UV = Input.UV;

    Output.Dir = transform_dir(float3(0, 1, 0));
    Output.Dir = normalize(Output.Dir);
    
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
    
    NormalMap.y = 1.0f - NormalMap.y;
    
    NormalMap = NormalMap * 2.0f - 1.0f;
    
    float3x3 TBN = float3x3(Input.TBN0, Input.TBN1, Input.TBN2);
    float3 Normal = normalize(mul(NormalMap, TBN));
    float3 FlatNormal = normalize(mul(float3(0, 0, 1), TBN));
        
    float3 LightPosition = float3(1.0f, 20.0f, 10.0f);
    float3 LightColor = float3(1.0f, 1.0f, 1.0f);
    float3 LightDirection = Input.Dir; //normalize(LightPosition - Input.Position);
    //LightDirection = normalize(LightDirection);
    float AmbientBrightness = 0.1f;
    float SphericalHarmonicsApproximation = saturate(max(Normal.y, 0.0f) + 0.5f);
    
    LightComponents Light = Calculate_Lighting_Model(Roughness, Metallic, Albedo, Input.Position, Normal, LightDirection);
    
    float NdotL = safe_dot(Normal, LightDirection);
    float SelfShadowing = safe_dot(FlatNormal, LightDirection);
    float3 DirectLighting = (Light.Diffuse + Light.Specular) * SelfShadowing; //NdotL * SelfShadowing * LightColor * Albedo;
    float3 IndirectLighting = AmbientBrightness * AO * SphericalHarmonicsApproximation * Albedo;
    float3 FinalColor = (IndirectLighting + DirectLighting);
    //FinalColor = LumaWeightedReinhard(FinalColor);
    FinalColor = SelfShadowing; //LinearTosRgb(FinalColor);
    return FinalColor.xyzz;
}
///////////////////////////////////////////////////////////////
