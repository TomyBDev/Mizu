#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class WaterShader : public Shader
{
public:
	WaterShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~WaterShader();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMapTexture, ID3D11ShaderResourceView* waterTexture, DirectionalLight dirLight, class Camera* camera, float strength = 1.f);
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
		XMFLOAT3 position;
		XMFLOAT3 direction;
		XMFLOAT2 buffer;
	};

	struct ControlBufferType
	{
		float strength;
		XMFLOAT3 buffer;
	};

	//Vertex
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* heightMapSampleState;

	//Pixel
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* controlBuffer;
	ID3D11SamplerState* waterSampleState;
};