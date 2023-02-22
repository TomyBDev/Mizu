cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VS_Input
{
    float3 pos : POSITION;
    float3 normals : NORMALS;
};

struct VS_Output
{
    float4 pos : SV_POSITION;
    float4 normals : Normals1;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    float4 position = float4(input.pos.x, input.pos.y, input.pos.z, 0.f);
    output.normals = float4(0.f, 0.f, 0.f, 0.f);

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(position, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.normals.xyz = input.normals.xyz;

    return output;
}