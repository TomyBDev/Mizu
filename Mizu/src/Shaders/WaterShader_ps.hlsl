Texture2D depthTexture : register(t0);
TextureCube skyTextureCube : register(t1);
SamplerState depthSampler : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 lAmbient;
    float4 lDiffuse;
    float3 lDirection;
    float lBuffer;
};

cbuffer CameraBuffer : register(b1)
{
    float3 camPosition;
    float3 camDirection;
    float2 camBuffer;
};

cbuffer ControlBuffer : register(b2)
{
    float cStrength;
    uint reflection;
    float2 cBuffer;
    float4 shallowColor;
    float4 deepColor;
};

cbuffer MatrixBuffer : register(b3)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex : TEXTURE;
    float3 normals : NORMALS;
    float4 worldPos : WORLDPOS;
};

float4 main(PS_Input input) : SV_TARGET
{
    //return float4(abs(input.normals.x), input.normals.y, abs(input.normals.z), 1.f);

    // Calculate depth value
    const float depth = clamp(depthTexture.Sample(depthSampler, input.tex).x * cStrength, 0, 1);

    // Calculate depth color by linear interpolating between shallow color and deep color using depth value.
    const float4 depthColor = lerp(shallowColor, deepColor, depth);

    // Calculate incident ray using mesh vertex position and camera position.
    float3 eyeVector = normalize(input.worldPos.xyz - camPosition);

    // Calculate relfected ray
    float3 ray = reflect(eyeVector, normalize(input.normals));

    // Sample sky texture cube using reflected ray.
    const float3 reflectionColor = skyTextureCube.Sample(depthSampler, ray).xyz;

    /*float3 refractedRay = refract(eyeVector, normalize(input.normals), 1.f);

    float F0 = pow((1.33 - 1) / (1.33 + 1), 2);
    float frenselWeight = F0 + (1 - F0) * pow(1 - dot(normalize(input.normals), eyeVector), 5);

    const float3 light = lerp(refractionColor, reflectionColor, frenselWeight);*/

    // Combine depth color and reflected color
    if (reflection)
		return float4(lerp(depthColor.xyz, reflectionColor.xyz, 0.2f), depthColor.w);

    return depthColor;
}