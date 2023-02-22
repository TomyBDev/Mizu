struct PS_Input
{
    float4 pos : SV_POSITION;
    float3 normals : NORMALS;
};

float4 main(PS_Input input) : SV_TARGET
{
    return float4(1.f,0.f,0.f,1.f);
}