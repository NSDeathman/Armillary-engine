////////////////////////////////////////////////////////////////////////////
#include "common.ash"
////////////////////////////////////////////////////////////////////////////
// https://www.xrhadertoy.com/view/WscyRl
// https://habr.com/ru/articles/426123/
// https://habr.com/ru/articles/326852/
////////////////////////////////////////////////////////////////////////////
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

float3 fresnelSchlickRoughness(float3 F0, float cosTheta, float roughness)
{
    return F0 + (max(1.0 - roughness, F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
////////////////////////////////////////////////////////////////////////////
// Unreal engine 4
////////////////////////////////////////////////////////////////////////////
// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX(float a2, float NoH)
{
    float d = (NoH * a2 - NoH) * NoH + 1.0f;
    return a2 / (PI * d * d);
}

// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointApprox(float a2, float NoV, float NoL)
{
    float a = sqrt(a2);
    float Vis_SmithV = NoL * (NoV * (1 - a) + a);
    float Vis_SmithL = NoV * (NoL * (1 - a) + a);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}

// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJoint(float a2, float NoV, float NoL)
{
    float Vis_SmithV = NoL * sqrt(NoV * (NoV - NoV * a2) + a2);
    float Vis_SmithL = NoV * sqrt(NoL * (NoL - NoL * a2) + a2);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}
////////////////////////////////////////////////////////////////////////////
float getMipLevelFromRoughness(float roughness, float lodCount)
{
    float ROUGHNESS_1_MIP_RESOLUTION = 1.5;
    float deltaLod = lodCount - ROUGHNESS_1_MIP_RESOLUTION;
    return deltaLod * (sqrt(1.0 + 24.0 * roughness) - 1.0) / 4.0;
}
////////////////////////////////////////////////////////////////////////////
float3 getSpecularDominantDir(float3 rd, float3 n, float roughness)
{
    // The dominant direction of specular reflection for
    // rough materials is different from the "mirror" direction.
    // This is an approximation used in Frostbite for a GGX BRDF.
    // https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf
    float smoothness = clamp(1.0f - roughness, 0.0f, 1.0f);
    float lerpFactor = smoothness * (sqrt(smoothness) + roughness);
    return normalize(lerp(n, reflect(rd, n), lerpFactor));
}
////////////////////////////////////////////////////////////////////////////
// https://www.shadertoy.com/view/MsSczh
////////////////////////////////////////////////////////////////////////////
float Oren_Nayar_Diffuse(float3 ViewDirection,
                         float3 Normal,
                         float LdotV,
                         float NdotL,
                         float NdotV,
                         float Roughness)
{
    float s = LdotV - NdotL * NdotV;
    float t = lerp(1.0f, max(NdotL, NdotV), step(0.0f, s));

    float sigma2 = pow2(Roughness);
    float A = 1. - .5 * (sigma2 / ((sigma2 + .33) + .000001));
    float B = .45 * sigma2 / (sigma2 + .09) + .00001;
    
    float ga = dot(ViewDirection - Normal * NdotV, Normal - Normal * NdotL);

    return max(0.0f, NdotL) * (A + B * max(0.0f, ga) * sqrt((1.0f - NdotV * NdotV) * (1.0f - NdotL * NdotL)) / max(NdotL, NdotV));
}
////////////////////////////////////////////////////////////////////////////
// Subsurface scattering
////////////////////////////////////////////////////////////////////////////
//https://github.com/FlaxEngine/FlaxEngine/blob/master/Source/Shaders/Lighting.hlsl
/*
LightingData SubsurfaceShading(GBufferSample gBuffer, float energy, float3 L, float3 V, half3 N)
{
    LightingData lighting = StandardShading(gBuffer, energy, L, V, N);
#if defined(USE_GBUFFER_CUSTOM_DATA)
    // Fake effect of the light going through the material
    float3 subsurfaceColor = gBuffer.CustomData.rgb;
    float opacity = gBuffer.CustomData.a;
    float3 H = normalize(V + L);
    float inscatter = pow(saturate(dot(L, -V)), 12.1f) * lerp(3, 0.1f, opacity);
    float normalContribution = saturate(dot(N, H) * opacity + 1.0f - opacity);
    float backScatter = gBuffer.AO * normalContribution / (PI * 2.0f);
    lighting.Transmission = lerp(backScatter, 1, inscatter) * subsurfaceColor;
#endif
    return lighting;
}

LightingData FoliageShading(GBufferSample gBuffer, float energy, float3 L, float3 V, half3 N)
{
    LightingData lighting = StandardShading(gBuffer, energy, L, V, N);
#if defined(USE_GBUFFER_CUSTOM_DATA)
    // Fake effect of the light going through the thin foliage
    float3 subsurfaceColor = gBuffer.CustomData.rgb;
    float wrapNoL = saturate((-dot(N, L) + 0.5f) / 2.25);
    float VoL = dot(V, L);
    float scatter = D_GGX(0.36, saturate(-VoL));
    lighting.Transmission = subsurfaceColor * (wrapNoL * scatter);
#endif
    return lighting;
}
*/
////////////////////////////////////////////////////////////////////////////
//https://www.shadertoy.com/view/XXSBzd
////////////////////////////////////////////////////////////////////////////
#define constant1_FON (0.5f - 2.0f / (3.0f * PI))
#define constant2_FON (2.0f / 3.0f - 28.0f / (15.0f * PI))
////////////////////////////////////////////////////////////////////////////
float E_FON_approx2(float mu, float r)
{
    float mucomp = invert(mu);
    float mucomp2 = pow2(mucomp);
    const float2x2 Gcoeffs = float2x2(0.0571085289f, -0.332181442f, 0.491881867f, 0.0714429953f);
    float GoverPi = dot(mul(Gcoeffs, float2(mucomp, mucomp2)), float2(1.0f, mucomp2));
    return (1.0f + r * GoverPi);
}

float3 f_EON_Opt(float3 Albedo, float Roughness, float NdotL, float NdotV, float LdotV)
{
    float s = LdotV - NdotL * NdotV; // QON s term
    float sovertF = s > 0.0f ? s / max(NdotV, NdotL) : s; // FON s/t
    float AF = 1.0f / (1.0f + constant1_FON * Roughness); // FON A coeff.
    float3 f_ss = (Albedo) * AF * (1.0f + Roughness * sovertF); // single-scatter
    
    float EFo = E_FON_approx2(NdotV, Roughness) * AF; // FON wo albedo (exact)
    float EFi = E_FON_approx2(NdotL, Roughness) * AF;

    float avgEF = AF * (1.0f + constant2_FON * Roughness); // avg. albedo
    float3 rho_ms = (Albedo * Albedo) / (1.0f - Albedo * (1.0f - avgEF));
    const float eps = 1.0e-7f;
    
    // multi-scatter lobe:(removed the max on numerator as didn't appear to make any difference)
    float3 f_ms = (rho_ms) * (avgEF * (1.0f - EFo) * (1.0f - EFi) / max(eps, 1.0f - avgEF));
    
    return f_ss + f_ms;
}
////////////////////////////////////////////////////////////////////////////
// Taken from https://gist.github.com/romainguy/a2e9208f14cae37c579448be99f78f25
// Modified by Epic Games, Inc. To account for premultiplied light color and code style rules.
////////////////////////////////////////////////////////////////////////////
float GGX_Mobile(float Roughness, float NoH)
{
    // Walter et al. 2007, "Microfacet Models for Refraction through Rough Surfaces"
    float OneMinusNoHSqr = 1.0 - NoH * NoH;
    float a = Roughness * Roughness;
    float n = NoH * a;
    float p = a / (OneMinusNoHSqr + n * n);
    float d = p * p;
	// clamp to avoid overlfow in a bright env
    return min(d, 2048.0f);
}
////////////////////////////////////////////////////////////////////////////
// Trowbridge-Reitz AKA GGX
float distribution(float3 n, float3 h, float roughness)
{
    float a_2 = roughness * roughness;
    return a_2 / (PI * pow(pow(safe_dot(n, h), 2.0f) * (a_2 - 1.0f) + 1.0f, 2.0f));
}

// Schlick-Beckmann
float geometry(float cosTheta, float k)
{
    return (cosTheta) / (cosTheta * (1.0f - k) + k);
}

float smiths(float NdotV, float NdotL, float roughness)
{
    float k = pow(roughness + 1.0f, 2.0f) / 8.0f;
    return geometry(NdotV, k) * geometry(NdotL, k);
}
////////////////////////////////////////////////////////////////////////////
// Ultimate lighting model
////////////////////////////////////////////////////////////////////////////
#define USE_OREN_NAYAR_DIFFUSE 1
////////////////////////////////////////////////////////////////////////////
struct LightComponents
{
    float3 Diffuse;
    float3 Specular;
    float3 Subsurface;
};
//---------------------------------------------------------//
// Full PBR lighting model for metallic-roughness materials//
//---------------------------------------------------------//
LightComponents Calculate_Lighting_Model(float Roughness, float Metallic, float3 Albedo, float3 Point, float3 Normal, float3 LightDirection)
{
    LightComponents Light;

    Roughness = clamp(Roughness, 0.015f, 1.0f);
    Normal = normalize(Normal);
    LightDirection = -normalize(LightDirection);
    float3 ViewDirection = -normalize(Point);
    float3 HalfWayDirection = normalize(ViewDirection + LightDirection);
    float NdotL = safe_dot(Normal, LightDirection);
    float NdotV = safe_dot(Normal, ViewDirection);
    float LdotV = safe_dot(ViewDirection, LightDirection);
    float LdotH = safe_dot(LightDirection, HalfWayDirection);
    float NdotH = safe_dot(Normal, HalfWayDirection);
    float HdotV = safe_dot(HalfWayDirection, ViewDirection);
    float RoughnessSqr = pow2(Roughness);
    float RoughnessSqr2 = pow2(RoughnessSqr);

    // Get F0 
    float3 F0 = 0.04f;
    F0 = lerp(F0, Albedo, Metallic);
    float3 kS = fresnelSchlickRoughness(F0, HdotV, Roughness);
    float3 kD = (1.0f - kS) * (1.0f - Metallic);

    // Cook-Torrance specular model
    float D = D_GGX(RoughnessSqr2, NdotH) * NdotH / (4.0f * HdotV) * NdotL;
    float V = Vis_SmithJoint(RoughnessSqr, NdotV, saturate(NdotL));
    Light.Specular = D * V * kS;
    
#if DISABLE_SHADING
    Light.Diffuse = Albedo * NdotL * kD / PI;
#else
    Light.Diffuse = Oren_Nayar_Diffuse(ViewDirection, Normal, LdotV, NdotL, NdotV, Roughness) * Albedo * kD;
#endif

    // Fake effect of the light going through the thin foliage
    float wrapNoL = saturate((-NdotL + 0.5f) / 2.25f);
    float scatter = D_GGX(0.36f, saturate(-LdotV));
    Light.Subsurface = wrapNoL * scatter;

    return Light;
}
//---------------------------------------------------------//
// Fast PBR lighting model only for roughness based shading//
// Unreal Engine 4 code for mobile shading                 //
//---------------------------------------------------------//
LightComponents Calculate_Lighting_Model_Fast(float Roughness, float Metallic, float3 Albedo, float3 Point, float3 Normal, float3 LightDirection)
{
    LightComponents Light;

    Roughness = Roughness + 0.015f;
    Normal = normalize(Normal);
    LightDirection = -normalize(LightDirection);
    float3 ViewDirection = -normalize(Point);
    float3 HalfWayDirection = normalize(ViewDirection + LightDirection);
    float NdotL = max(dot(Normal, LightDirection), 0.00001f);
    float NdotH = saturate(dot(Normal, HalfWayDirection));
    float NdotV = max(dot(Normal, ViewDirection), 1e-5);
    float HdotV = saturate(dot(HalfWayDirection, ViewDirection));
    
    float F0 = 0.04f;
    float Fc = pow(1.0f - HdotV, 5.0f);
    float F = Fc + (1.0f - Fc) * F0;
    float kD = (1.0f - F);
    
    float a = Roughness * Roughness;
    float Vis_SmithV = NdotL * (NdotV * (1.0f - a) + a);
    float Vis_SmithL = NdotV * (NdotL * (1.0f - a) + a);
    float Vis = 0.5f * rcp(Vis_SmithV + Vis_SmithL);

    Light.Specular = NdotL * F * Vis * GGX_Mobile(Roughness, NdotH);
    Light.Diffuse = Albedo * NdotL / PI * 2.0f * kD;
    Light.Subsurface = NULL;

    return Light;
}
//----------------------------------------------//
// Lighting model for non shadowed light sources//
//----------------------------------------------//
LightComponents Calculate_Diffuse_Lighting_Model(float Roughness, float Metallic, float3 Albedo, float3 Point, float3 Normal, float3 LightDirection)
{
    LightComponents Light;

    Normal = normalize(Normal);
    LightDirection = -normalize(LightDirection);
    float3 ViewDirection = -normalize(Point);
    float NdotL = max(dot(Normal, LightDirection), 0.00001f);
    float NdotV = max(dot(Normal, ViewDirection), 1e-5);
    float LdotV = dot(ViewDirection, LightDirection);
    float3 HalfWayDirection = normalize(ViewDirection + LightDirection);
    float LdotH = dot(LightDirection, HalfWayDirection);
    
#if DISABLE_SHADING
    Light.Diffuse = Albedo * NdotL;
#else
    Light.Diffuse = Oren_Nayar_Diffuse(ViewDirection, Normal, LdotV, NdotL, NdotV, Roughness) * Albedo;
#endif

    Light.Specular = NULL;
    Light.Subsurface = NULL;

    return Light;
}
////////////////////////////////////////////////////////////////////////////
// Spot\Point lighting
////////////////////////////////////////////////////////////////////////////
float CalculateAttenuation(float3 Point, float3 LightPosition, float LightSourceRange)
{
    float3 LightDirection = Point - LightPosition;
    float LightRadiusSquared = dot(LightDirection, LightDirection);
    return saturate(1.0f - LightRadiusSquared * LightSourceRange);
}
////////////////////////////////////////////////////////////////////////////