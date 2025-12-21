cbuffer WorldBuffer : register(b0)
{
    float4x4 World;
    float4x4 ViewProjection;
    float4x4 WorldViewProjection;
};

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Bitangent : BITANGENT;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0;
    float3 WorldPos : TEXCOORD1;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // Преобразование в мировые координаты
    float4 worldPos = mul(float4(input.Position, 1.0f), World);

    // Преобразование в пространство проекции
    output.Position = mul(worldPos, ViewProjection);

    // Преобразование нормали
    float3x3 worldNormalMatrix = (float3x3)World;
    output.Normal = normalize(mul(input.Normal, worldNormalMatrix));

    output.UV = input.UV;
    output.WorldPos = worldPos.xyz;

    return output;
}

// Пиксельный шейдер остается тем же
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

float4 PS(PS_INPUT input) : SV_TARGET
{
    float4 albedo = txDiffuse.Sample(samLinear, input.UV);

    // Простое освещение
    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
    float diff = max(dot(input.Normal, lightDir), 0.2f);

    float3 ambient = float3(0.2f, 0.2f, 0.2f);
    float3 finalColor = albedo.rgb * (ambient + diff);

    return float4(finalColor, 1.0f);
}
