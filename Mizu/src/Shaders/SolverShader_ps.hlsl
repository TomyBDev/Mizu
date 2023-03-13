Texture2D solverTexture : register(t0);
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

float3 ColorToSpeed(float3 c)
{
    return float3(c.x, c.yz - 0.5f);
}

float3 SpeedToColor(float3 s)
{
    return float3(s.x, s.yz + 0.5f);
}

float3 F(float3 u)
{
    float val1 = u.x * u.y;
    float val2 = u.x * u.y * u.y + 0.5f * 9.8f * u.x * u.x;
    float val3 = u.x * u.y * u.z;

    return float3(val1, val2, val3);
}

float3 G(float3 u)
{
    float val1 = u.x * u.z;
    float val2 = u.x * u.y * u.z;
    float val3 = u.x * u.z * u.z + 0.5f * 9.81f * u.x * u.x;

    return float3(val1, val2, val3);
}

float4 main(PS_Input input) : SV_TARGET
{
    // If we are on boundary
    if (input.tex.x == 0.f || input.tex.x == 1.f || input.tex.y == 0.f || input.tex.y == 1.f)
        return solverTexture.Sample(solverSampler, input.tex);

    //const float dx = 1.f / 100.f;
    const float dx = 1.f;
    const float dt2 = 0.00416666667;

	const float3 u = solverTexture.Sample(solverSampler, input.tex).xyz;
	const float3 un = solverTexture.Sample(solverSampler, input.tex + float2(0, dx)).xyz;
    const float3 ue = solverTexture.Sample(solverSampler, input.tex + float2(dx, 0)).xyz;
    const float3 us = solverTexture.Sample(solverSampler, input.tex + float2(0, -dx)).xyz;
    const float3 uw = solverTexture.Sample(solverSampler, input.tex + float2(-dx, 0)).xyz;

    const float3 newU = 0.25f * (un + ue + us + uw) - ((dt2 / (2 * dx)) * (F(ue) - F(uw))) - ((dt2 / (2 * dx)) * (G(un) - G(us)));

    return float4(newU, 1.f);
}