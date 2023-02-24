Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer dataBuffer : register(b0)
{
    bool bFirstFrame;
    bool pad[3];
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normals : NORMALS;
};

float4 main(PS_Input input) : SV_TARGET
{
    float4 color;

    if (bFirstFrame)
    {
        color = texture0.Sample(sampler0, input.tex);
    }
    else if (input.tex.x > 0.f || input.tex.x < 1.f)
    {
        color = texture0.Sample(sampler0, input.tex * 0.0844594595);
    }
    return color;
}