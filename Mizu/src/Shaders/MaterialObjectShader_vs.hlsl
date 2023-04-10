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
    float3 ambient : AMBIENT;
    float3 diffuse : DIFFUSE;
    float3 emissive : EMISSIVE;
    float3 specular : SPECULAR;
    float specExponent : SPECEXPONENT;
    int diffID : DIFFID;
    int bumpID : BUMPID;
    float bumpMult : BUMPMULT;
};

struct VS_Output
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
    float3 worldPos : WORLDPOS;
    float3 ambient : AMBIENT;
    float3 diffuse : DIFFUSE;
    float3 emissive : EMISSIVE;
    float3 specular : SPECULAR;
    float specExponent : SPECEXPONENT;
    int diffID : DIFFID;
    int bumpID : BUMPID;
    int bumpMult : BUMPMULT;
};

VS_Output main(VS_Input input)
{
    VS_Output output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.worldPos = mul(input.pos, worldMatrix);

    output.normals = mul(input.normals, (float3x3) worldMatrix);
    output.normals = normalize(output.normals);

    output.tex = input.tex;

    output.ambient = input.ambient;
    output.diffuse = input.diffuse;
    output.specular = input.specular;
    output.specExponent = input.specExponent;
    output.diffID = input.diffID;
    output.bumpID = input.bumpID;

    return output;
}