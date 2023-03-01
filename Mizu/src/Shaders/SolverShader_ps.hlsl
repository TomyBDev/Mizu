Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer ScaleBuffer : register(b0)
{
    float scale;
    float buffer[3];
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normals : NORMALS;
};

float4 main(PS_Input input) : SV_TARGET
{
    return texture0.Sample(sampler0, input.tex * scale);
}