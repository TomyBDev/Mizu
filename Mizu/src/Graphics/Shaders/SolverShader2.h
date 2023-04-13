#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class SolverShader2 : public Shader
{
public:
	SolverShader2(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const wchar_t* vsFileName, const wchar_t* psFileName);
	~SolverShader2();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* oldTexture, ID3D11ShaderResourceView* pass1Texture, float dt, std::pair<int, int> resolution);
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
		int resX;
		int resZ;
		float padding;
	};

	//Vertex
	ID3D11Buffer* matrixBuffer;

	//Pixel
	ID3D11Buffer* dataBuffer;
	ID3D11SamplerState* solverSampleState;
};