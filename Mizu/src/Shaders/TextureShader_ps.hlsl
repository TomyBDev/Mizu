Texture2D theTexture : register(t0);
SamplerState textureSampler : register(s1);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
};

float4 main(PS_Input input) : SV_TARGET
{
    return theTexture.Sample(textureSampler, input.tex) * 0.0390625f; // Number here is to scale down the image file values to something smaller.
}