struct InputType
{
    float4 pos : POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct OutputType
{
    float4 pos : POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;

    output.edges[0] = 3.0f;
    output.edges[1] = 3.0f;
    output.edges[2] = 3.0f;
    output.inside = 3.0f;

    return output;
}


[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    output.pos = patch[pointId].pos;
    output.normals = patch[pointId].normals;
    output.tex = patch[pointId].tex;

    return output;
}