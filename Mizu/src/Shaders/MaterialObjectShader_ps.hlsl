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
    float3 materials : MATERIALS;
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

    return float4(saturate(lightColor).xyz * input.materials, 1.f);
}