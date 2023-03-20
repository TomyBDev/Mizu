TextureCube skyTextureCube : register(t0);
SamplerState textureSampler : register(s0);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 tex : TEXTURE;
    float3 normals : NORMALS;
};

float4 main(PS_Input input) : SV_TARGET
{

    return skyTextureCube.Sample(textureSampler, input.tex);
    	
}