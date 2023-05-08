Texture2D oldTexture : register(t0);
Texture2D pass1Texture : register(t1);
SamplerState solverSampler : register(s0);

cbuffer DataBuffer : register(b0)
{
    float dt;
    int resX;
    int resZ;
    float padding;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

float3 F(float3 u)
{
    float val1 = u.y;
    float val2 = ((u.y * u.y) / u.x) + (0.5f * 9.81f * u.x * u.x);
    float val3 = (u.y * u.z) / u.x;

    return float3(val1, val2, val3);
}

float3 G(float3 u)
{
    float val1 = u.z;
    float val2 = (u.y * u.z) / u.x;
    float val3 = ((u.z * u.z) / u.x) + (0.5f * 9.81f * u.x * u.x);

    return float3(val1, val2, val3);
}

float4 Reflect(float2 uv, float2 offset)
{
    return float4(oldTexture.Sample(solverSampler, uv + offset).xyz * float3(1.f, -1.f, -1.f), 1.f);
}

float4 main(PS_Input input) : SV_TARGET
{
    const float du = 1.f / resX;
    const float dv = 1.f / resZ;


    // Time and Displacement Step
    const float dx = 0.64f;
    const float dt2 = 0.5f;

    // Get neighbour values from previous pass.
    const float3 uOld = oldTexture.Sample(solverSampler, input.tex).xyz;
    const float3 un = (input.tex.y + dv) <= 1.f ? pass1Texture.Sample(solverSampler, input.tex + float2(0, dv)).xyz : pass1Texture.Sample(solverSampler, input.tex).rgb * float3(1, 1, 0);
    const float3 ue = (input.tex.x + du) <= 1.f ? pass1Texture.Sample(solverSampler, input.tex + float2(du, 0)).xyz : pass1Texture.Sample(solverSampler, input.tex).rgb * float3(1, 0, 1);
    const float3 us = (input.tex.y - dv) >= 0.f ? pass1Texture.Sample(solverSampler, input.tex + float2(0, -dv)).xyz : pass1Texture.Sample(solverSampler, input.tex).rgb * float3(1, 1, 0);
    const float3 uw = (input.tex.x - du) >= 0.f ? pass1Texture.Sample(solverSampler, input.tex + float2(-du, 0)).xyz : pass1Texture.Sample(solverSampler, input.tex).rgb * float3(1, 0, 1);

    // Calculate New values
    const float3 val2 = (dt2 / (2 * dx)) * (F(ue) - F(uw));
    const float3 val3 = (dt2 / (2 * dx)) * (G(un) - G(us));
    const float3 newU = uOld - val2 - val3;

    // Return new pixel color (storing the height and momentum in texture).
    return float4(newU, 1.f);
}