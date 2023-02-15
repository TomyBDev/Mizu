#pragma once

class Shader
{
public:
	Shader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
		: device(dev),
		deviceContext(context) {}

	~Shader();

	void Render(int indexCount);

protected:

	void LoadVertexShader(const wchar_t* fileName);
	void LoadPixelShader(const wchar_t* fileName);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};