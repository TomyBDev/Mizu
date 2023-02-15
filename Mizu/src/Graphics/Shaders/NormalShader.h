#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class NormalShader : public Shader
{
public:
	NormalShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~NormalShader();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};