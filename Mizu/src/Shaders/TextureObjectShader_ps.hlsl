Texture2D diffuseTexture : register(t0);
Texture2D specularTexture : register(t1);
SamplerState diffuseSampler : register(s0);
SamplerState specularSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 lAmbient;
    float4 lDiffuse;
    float3 lDirection;
    float lBuffer;
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
    const float4 lightColor = lAmbient + CalculateLighting(-lDirection, input.normals, lDiffuse);
    const float4 diffuse = diffuseTexture.Sample(diffuseSampler, input.tex);
    return diffuse;

    return float4(saturate(lightColor).xyz + diffuse.xyz, diffuse.w);
}