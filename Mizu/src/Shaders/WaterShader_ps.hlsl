Texture2D depthTexture : register(t0);
SamplerState depthSampler : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 lAmbient;
    float4 lDiffuse;
    float3 lDirection;
    float lBuffer;
};

cbuffer CameraBuffer : register(b1)
{
    float3 camPosition;
    float3 camDirection;
    float2 camBuffer;
};

cbuffer CameraBuffer : register(b2)
{
    float cStrength;
    float3 cBuffer;
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
    //saturate(lightColor)
    float4 deepColor = float4(0.2578125f, 0.421875f, 0.95703125f, 0.76f);
    float4 shallowColor = float4(0.2578125f, 0.8046875f, 0.95703125f, 0.51f);
    float depth = clamp(depthTexture.Sample(depthSampler, input.tex).x * cStrength, 0, 1);

    return lerp(shallowColor, deepColor, depth);
}