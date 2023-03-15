Texture2D theTexture : register(t0);
SamplerState normalTextureSampler : register(s0);
SamplerState copyTextureSampler : register(s1);

cbuffer DataBuffer : register(b0)
{
    float uvScale;
    bool copyMode;
    bool buffer1[3];
    float2 buffer2;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

float4 main(PS_Input input) : SV_TARGET
{
    if (copyMode)
        return theTexture.Sample(copyTextureSampler, input.tex) * 0.0390625f;

    return theTexture.Sample(normalTextureSampler, input.tex * uvScale);
    	
}