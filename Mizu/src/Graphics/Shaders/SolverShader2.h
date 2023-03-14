#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class SolverShader2 : public Shader
{
public:
	SolverShader2(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~SolverShader2();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* oldTexture, ID3D11ShaderResourceView* pass1Texture, float dt, int resolution);
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct DataBufferType
	{
		float dt;
		int res;
		XMFLOAT2 buffer;
	};

	//Vertex
	ID3D11Buffer* matrixBuffer;

	//Pixel
	ID3D11Buffer* dataBuffer;
	ID3D11SamplerState* solverSampleState;
};