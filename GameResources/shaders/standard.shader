// =================================================================================
// Constant Buffers
// =================================================================================

// Данные кадра (обновляются 1 раз за кадр) - регистр b0
cbuffer FrameBuffer : register(b0)
{
    row_major float4x4 View;       // 64 bytes
    row_major float4x4 Projection; // 64 bytes
    float3 CameraPos;              // 12 bytes
    float Padding;                 // 4 bytes (для выравнивания 16 байт)
};

// Данные объекта (обновляются для каждого меша) - регистр b1
cbuffer ObjectBuffer : register(b1)
{
    row_major float4x4 World;      // 64 bytes
};

// =================================================================================
// Input / Output Structures
// =================================================================================

struct VS_INPUT
{
    float3 Position : POSITION;
    // Остальные поля можно оставить, даже если не используем, 
    // чтобы InputLayout совпадал с C++
    float3 Normal   : NORMAL;
    float2 UV       : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
};

// =================================================================================
// Vertex Shader
// =================================================================================
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // 1. Local Space -> World Space
    // Умножаем вектор на матрицу (так как row_major)
    float4 worldPos = mul(float4(input.Position, 1.0f), World);

    // 2. World Space -> View Space
    float4 viewPos = mul(worldPos, View);

    // 3. View Space -> Clip Space
    output.Position = mul(viewPos, Projection);

    return output;
}

// =================================================================================
// Pixel Shader
// =================================================================================
float4 PS(PS_INPUT input) : SV_TARGET
{
    // Возвращаем просто КРАСНЫЙ цвет для теста
    // (R, G, B, A)
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
