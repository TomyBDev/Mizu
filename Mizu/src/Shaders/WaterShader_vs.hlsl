Texture2D heightMapTexture : register(t0);
SamplerState heightMapSampler : register(s0);

struct VS_Input
{
    float4 pos : POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

struct VS_Output
{
    float4 pos : POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    const float heightScale = 2000.f;

    //offset the y position based on the height map information
    input.pos.y = heightMapTexture.SampleLevel(heightMapSampler, input.tex, 0).x * heightScale;

    output.pos = input.pos;

    output.normals = input.normals;

    output.tex = input.tex;

    return output;
}