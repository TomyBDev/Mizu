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

VS_Output main(VS_Input input)
{
    VS_Output output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.normals = mul(input.normals, (float3x3) worldMatrix);
    output.normals = normalize(output.normals);

    output.tex = input.tex;

    return output;
}