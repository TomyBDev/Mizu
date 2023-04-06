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

float3 CalcNormals(float2 uv, float yPos, float scale)
{
	//recalc normals
    const float e = heightMapTexture.SampleLevel(heightMapSampler, float2(uv.x + (1.f / 1024.f), uv.y), 0).x * scale;
    const float w = heightMapTexture.SampleLevel(heightMapSampler, float2(uv.x - (1.f / 1024.f), uv.y), 0).x * scale;
    const float n = heightMapTexture.SampleLevel(heightMapSampler, float2(uv.x, uv.y + (1.f / 1024.f)), 0).x * scale;
    const float s = heightMapTexture.SampleLevel(heightMapSampler, float2(uv.x, uv.y - (1.f / 1024.f)), 0).x * scale;

    const float3 vecn = float3(0, n - yPos, 1);
    const float3 vece = float3(1, e - yPos, 0);
    const float3 vecs = float3(0, s - yPos, -1);
    const float3 vecw = float3(-1, w - yPos, 0);

    return (cross(vecn, vece) + cross(vece, vecs) + cross(vecs, vecw) + cross(vecw, vecn)) / 4.f;
}

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 3> patch)
{
    OutputType output;

    const float4 vertexPosition = uvwCoord.x * patch[0].pos + uvwCoord.y * patch[1].pos + uvwCoord.z * patch[2].pos;

    const float2 tex = uvwCoord.x * patch[0].tex + uvwCoord.y * patch[1].tex + uvwCoord.z * patch[2].tex;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(vertexPosition, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    // Normals
    output.normals = mul(CalcNormals(tex, vertexPosition.y, 2000.f), (float3x3) worldMatrix);
    output.normals = normalize(output.normals);

    // Send the input tex into the pixel shader.
    output.tex = tex;

    output.worldPos = mul(vertexPosition, worldMatrix);
    
    return output;
}