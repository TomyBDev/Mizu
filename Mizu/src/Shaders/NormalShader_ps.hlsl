Texture2D theTexture : register(t0);
SamplerState textureSampler : register(s0);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

float4 main(PS_Input input) : SV_TARGET
{
    return theTexture.Sample(textureSampler, input.tex);
}