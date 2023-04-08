Texture2D heightMapTexture : register(t0);
SamplerState heightMapSampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct InputType
{
    float4 pos : POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

struct OutputType
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
    float4 worldPos : WORLDPOS;
};

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 3> patch)
{
    OutputType output;

    const float4 vertexPosition = uvwCoord.x * patch[0].pos + uvwCoord.y * patch[1].pos + uvwCoord.z * patch[2].pos;

    const float2 tex = uvwCoord.x * patch[0].tex + uvwCoord.y * patch[1].tex + uvwCoord.z * patch[2].tex;

    const float3 normals = uvwCoord.x * patch[0].normals + uvwCoord.y * patch[1].normals + uvwCoord.z * patch[2].normals;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(vertexPosition, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    // Normals
    output.normals = normals;

    // Send the input tex into the pixel shader.
    output.tex = tex;

    output.worldPos = vertexPosition;
    
    return output;
}