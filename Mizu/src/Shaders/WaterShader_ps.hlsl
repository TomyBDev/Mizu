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
    float3 cBuffer;
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

    // Combine depth color and reflected color
    return float4(lerp(depthColor.xyz, reflectionColor, 0.25f), depthColor.w);
}