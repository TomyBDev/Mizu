#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class WaterShader : public Shader
{
public:
	WaterShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~WaterShader();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMapTexture, ID3D11ShaderResourceView* waterTexture, DirectionalLight dirLight);
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

	//Vertex
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* heightMapSampleState;

	//Pixel
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* waterSampleState;
};