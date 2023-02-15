#pragma once
#include <Graphics/Shader.h>

class NormalShader : public Shader
{
public:
	NormalShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

	//void SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
private:

};