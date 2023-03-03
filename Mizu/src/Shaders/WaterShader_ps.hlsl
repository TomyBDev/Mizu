Texture2D waterTexture : register(t0);
SamplerState waterSampler : register(s0);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

float4 main(PS_Input input) : SV_TARGET
{
    return waterTexture.Sample(waterSampler, input.tex * 5.f);
}