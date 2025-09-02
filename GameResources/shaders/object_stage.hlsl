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
};
///////////////////////////////////////////////////////////////
Interpolators VSMain(VertexData Input)
{
    Interpolators Output;
    
    Output.HomogeniousPosition = mul(float4(Input.Position, 1.0f), matWorldViewProjection);
    
    Output.Position = mul(float4(Input.Position, 1.0f), matWorld);

    float3 Binormal = cross(Input.Tangent, Input.Normal);

    float3x3 TBN = mul((float3x3)matWorld, float3x3(Input.Tangent.x, Binormal.x, Input.Normal.x,
                                                    Input.Tangent.y, Binormal.y, Input.Normal.y,
                                                    Input.Tangent.z, Binormal.z, Input.Normal.z));


    Output.TBN0 = TBN[0];
    Output.TBN1 = TBN[1];
    Output.TBN2 = TBN[2];
    
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
    
    float3x3 TBN = float3x3(Input.TBN0, Input.TBN1, Input.TBN2);
    
    float3 Normal = mul(TBN, NormalMap);
    
    Normal = normalize(Normal);
        
    float3 LightPosition = float3(1.0f, 5.0f, 10.0f);
    float3 LightColor = float3(1.0f, 0.9f, 1.0f);
    float3 AmbientBrightness = 0.1f;
    float3 LightDirection = normalize(Input.Position - LightPosition);
    
    float3 DirectLighting = safe_dot(Normal, float3(0.1, 0.2, 1)) * LightColor * Albedo;
    float3 IndirectLighting = AmbientBrightness * AO * Albedo;
    float3 FinalColor = (IndirectLighting + DirectLighting);
    //FinalColor = LumaWeightedReinhard(FinalColor);
    FinalColor = LinearTosRgb(FinalColor);
    return FinalColor.xyzz;
}
///////////////////////////////////////////////////////////////
