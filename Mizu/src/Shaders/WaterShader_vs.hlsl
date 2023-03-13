Texture2D heightMapTexture : register(t0);
SamplerState heightMapSampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VS_Input
{
    float4 pos : POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

struct VS_Output
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

float3 CalcNormals(float2 uv, float yPos, float scale)
{
	//recalc normals
    const float e = heightMapTexture.SampleLevel(heightMapSampler, float2(uv.x + (1.f /100.f), uv.y), 0).x * scale;
    const float w = heightMapTexture.SampleLevel(heightMapSampler, float2(uv.x - (1.f / 100.f), uv.y), 0).x * scale;
    const float n = heightMapTexture.SampleLevel(heightMapSampler, float2(uv.x, uv.y + (1.f / 100.f)), 0).x * scale;
    const float s = heightMapTexture.SampleLevel(heightMapSampler, float2(uv.x, uv.y - (1.f / 100.f)), 0).x * scale;

    const float3 vecn = float3(0, n - yPos, 1);
    const float3 vece = float3(1, e - yPos, 0);
    const float3 vecs = float3(0, s - yPos, -1);
    const float3 vecw = float3(-1, w - yPos, 0);

    return (cross(vecn, vece) + cross(vece, vecs) + cross(vecs, vecw) + cross(vecw, vecn)) / 4.f;
}


VS_Output main(VS_Input input)
{
    VS_Output output;
    const float heightScale = 250.f;

    //offset the y position based on the height map information
    input.pos.y = heightMapTexture.SampleLevel(heightMapSampler, input.tex, 0).x * heightScale;
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.normals = mul(CalcNormals(input.tex, input.pos.y, heightScale), (float3x3)worldMatrix);
    output.normals = normalize(output.normals);

    output.tex = input.tex;

    return output;
}