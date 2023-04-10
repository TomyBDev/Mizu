#pragma once
#include <Graphics/Shader.h>

using namespace DirectX;

class WaterShader : public Shader
{
public:
	WaterShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	~WaterShader();
	void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMapTexture, ID3D11ShaderResourceView* skyTextureCube, DirectionalLight dirLight, class Camera* camera, float* shallowColor, float* deepColor, float strength = 1.f, std::pair<int, int> res = { 128,128 });
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
		XMFLOAT4 shallowColor;
		XMFLOAT4 deepColor;
	};

	struct TessellationBufferType
	{
		int first;
		int second;
		XMFLOAT2 buffer;
	};

	struct ResolutionBufferType
	{
		int resX;
		int resY;
		XMFLOAT2 buffer;
	};

	//Vertex
	ID3D11Buffer* resolutionBuffer;

	ID3D11SamplerState* heightMapSampleState;

	//Hull
	ID3D11Buffer* tessellationBuffer;

	//Domain
	ID3D11Buffer* matrixBuffer;

	//Pixel
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* controlBuffer;

	ID3D11SamplerState* skySampleState;
};