struct PS_Input
{
    float4 pos : Position;
    float4 normals : Normals;
};

float4 main(PS_Input input) : SV_Target
{
    return input.normals;
}