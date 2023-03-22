#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class TextureObjectShader : public Shader
{
public:
	TextureObjectShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~TextureObjectShader();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, DirectionalLight dirLight, ID3D11ShaderResourceView* diffuseTex, ID3D11ShaderResourceView* normalTex, ID3D11ShaderResourceView* specularTex);
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
		float buffer;
	};

	//Vertex
	ID3D11Buffer* matrixBuffer;

	//Pixel
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* diffuseSampleState;
	ID3D11SamplerState* normalSampleState;
};