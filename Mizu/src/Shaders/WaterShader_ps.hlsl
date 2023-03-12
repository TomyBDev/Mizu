Texture2D waterTexture : register(t0);
SamplerState waterSampler : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float3 direction;
    float padding;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 CalculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    const float intensity = saturate(dot(normal, lightDirection));
    return saturate(diffuse * intensity);
}

float4 main(PS_Input input) : SV_TARGET
{
    const float4 color = ambient + CalculateLighting(-direction, input.normals, diffuse);
    return saturate(color) * waterTexture.Sample(waterSampler, input.tex);
}