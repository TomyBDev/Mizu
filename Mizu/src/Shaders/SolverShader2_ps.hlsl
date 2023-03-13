Texture2D oldTexture : register(t0);
Texture2D pass1Texture : register(t1);
SamplerState solverSampler : register(s0);

cbuffer DataBuffer : register(b0)
{
    float dt;
    float3 buffer;
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

float4 main(PS_Input input) : SV_TARGET
{
    // If we are on boundary
    if (input.tex.x == 0.f || input.tex.x == 1.f || input.tex.y == 0.f || input.tex.y == 1.f)
        return float4(0.f, 0.f, 0.f, 1.f);

    const float dx = 1.f;
    const float dt2 = 0.00416666667 * 25.f;
    const float du = 1.f / 100.f;

    const float3 uOld = oldTexture.Sample(solverSampler, input.tex).xyz;
    const float3 un = pass1Texture.Sample(solverSampler, input.tex + float2(0, du)).xyz;
    const float3 ue = pass1Texture.Sample(solverSampler, input.tex + float2(du, 0)).xyz;
    const float3 us = pass1Texture.Sample(solverSampler, input.tex + float2(0, -du)).xyz;
    const float3 uw = pass1Texture.Sample(solverSampler, input.tex + float2(-du, 0)).xyz;

    const float3 val2 = (dt2 / (2 * dx)) * (F(ue) - F(uw));
    const float3 val3 = (dt2 / (2 * dx)) * (G(un) - G(us));

    const float3 newU = uOld - val2 - val3;

    return float4(newU, 1.f);
}