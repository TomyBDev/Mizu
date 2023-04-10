#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class MaterialObjectShader : public Shader
{
public:
	MaterialObjectShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~MaterialObjectShader();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, DirectionalLight dirLight, ID3D11ShaderResourceView* diffSrv, ID3D11ShaderResourceView* normSrv, XMVECTOR camPos);
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

	struct CameraBufferType
	{
		XMFLOAT3 camPos;
		float padding;
	};

	//Vertex
	ID3D11Buffer* matrixBuffer;

	//Pixel
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11SamplerState* normalSampleState;
	ID3D11SamplerState* copySampleState;
};