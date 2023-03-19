Texture2D depthTexture : register(t0);
TextureCube skyTextureCube : register(t1);
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

cbuffer ControlBuffer : register(b2)
{
    float cStrength;
    float3 cBuffer;
    float4 shallowColor;
    float4 deepColor;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

float4 main(PS_Input input) : SV_TARGET
{
    const float depth = clamp(depthTexture.Sample(depthSampler, input.tex).x * cStrength, 0, 1);


	return lerp(shallowColor, deepColor, depth);

    //return skyTextureCube.Sample(depthSampler, -lDirection);
}