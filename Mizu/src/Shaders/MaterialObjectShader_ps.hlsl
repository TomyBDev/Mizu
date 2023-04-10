Texture2DArray diffuseTextures : register(t0);
Texture2DArray bumpTextures : register(t1);
SamplerState normalTextureSampler : register(s0);
SamplerState copyTextureSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 lAmbient;
    float4 lDiffuse;
    float3 lDirection;
    float lBuffer;
};

cbuffer CamBuffer : register(b1)
{
    float3 camPos;
    float padding;
};

struct PS_Input
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

float3 CalculateDiffuse(float3 lightDir, float3 normal, float3 lightDiff)
{
    return max(0.f, dot(normal, lightDir)) * lightDiff; 
}

float3 CalculateSpecular(float3 eyeDir, float3 lightDir, float3 normal, float3 lightDiff, float specExponent)
{
    float3 R = normalize(reflect(-lightDir, normal));
    float RdotV = max(0, dot(R, eyeDir));

    float3 H = normalize(lightDir + eyeDir);
    float NdotH = max(0, dot(normal, H));

    return lightDir * pow(RdotV, specExponent);

}

float4 main(PS_Input input) : SV_TARGET
{
    // Get Normals

    float3 normals = input.normals;

    if (input.bumpID >= 0)
        normals += input.bumpMult * bumpTextures.Sample(copyTextureSampler, float3(input.tex, input.bumpID)).xyz;

    // Calc stuff or something

    float3 V = normalize(camPos - input.worldPos.xyz);

    const float3 diffuse = saturate(CalculateDiffuse(-lDirection, normals, lDiffuse.xyz));
    const float3 specular = saturate(CalculateSpecular(V, -lDirection, normals, lDiffuse.xyz, input.specExponent));

    // Ambient Color

    const float3 ambientColor = input.ambient * 0.1f; 

    // Diffuse Color

    const float3 diffuseColor = input.diffuse * diffuse;


    // Specular

    const float3 specularColor = input.specular * specular;

	// Texture Color

    float3 textureColor = float3(1.f, 1.f, 1.f);

    if (input.diffID >= 0)
        textureColor = diffuseTextures.Sample(normalTextureSampler, float3(input.tex, input.diffID)).xyz;


    // Final Color

    float3 finalColor = (input.emissive + ambientColor + diffuseColor + specularColor) * textureColor;

    // Return

    return float4(finalColor, 1.f);
}