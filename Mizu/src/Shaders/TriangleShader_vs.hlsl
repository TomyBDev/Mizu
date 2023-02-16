cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

float4 main(float4 pos : Position) : SV_Position
{
    float4 position = {0,0,0,1};

	// Calculate the position of the vertex against the world, view, and projection matrices.
    position = mul(pos, worldMatrix);
    position = mul(position, viewMatrix);
    position = mul(position, projectionMatrix);

    return position;
}