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

    // Left Boundary
    if (input.tex.x == 0.f)
        return Reflect(input.tex, float2(du, 0.f));
    // Right Boundary
    if (input.tex.x == 1.f)
		return Reflect(input.tex, float2(-du, 0.f));
    // Top Boundary
	if (input.tex.y == 0.f)
		return Reflect(input.tex, float2(0.f, dv));
    // Bottom Boundary
	if (input.tex.y == 1.f)
		return Reflect(input.tex, float2(0.f, -dv));
    
    // Time and Displacement Step
    const float dx = resX * 0.004f;
    const float dt2 = 0.000813802084*resX;

    // Get neighbour values from previous pass.
    const float3 uOld = oldTexture.Sample(solverSampler, input.tex).xyz;
    const float3 uHalf = pass1Texture.Sample(solverSampler, input.tex).xyz;
    const float3 us = pass1Texture.Sample(solverSampler, input.tex + float2(0, -dv)).xyz;
    const float3 uw = pass1Texture.Sample(solverSampler, input.tex + float2(-du, 0)).xyz;

    // Calculate New values
    const float3 val1 = 0.5f * (uOld + uHalf);
    const float3 val2 = (dt2 / (2 * dx)) * (F(uHalf) - F(uw));
    const float3 val3 = (dt2 / (2 * dx)) * (G(uHalf) - G(us));
    const float3 newU = val1 - val2 - val3;

    // Return new pixel color (storing the height and momentum in texture).
    return float4(newU, 1.f);
}