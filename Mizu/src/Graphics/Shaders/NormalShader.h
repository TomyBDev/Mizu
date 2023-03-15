#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class NormalShader : public Shader
{
public:
	NormalShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~NormalShader();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float uvScale = 1.f, bool bCopyMode = false);
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct DataBufferType
	{
		float uvScale;
		bool copyMode;
		bool buffer1[3];
		XMFLOAT2 buffer2;
	};

	//Vertex
	ID3D11Buffer* matrixBuffer;

	//Pixel
	ID3D11Buffer* dataBuffer;
	ID3D11SamplerState* normalSampleState;
	ID3D11SamplerState* copySampleState;
};