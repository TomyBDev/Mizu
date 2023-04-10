#pragma once

class TextureArray
{
public:
	TextureArray(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::vector<std::wstring>* fileNames);

	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	Gdiplus::Color** pixels;
	int size;
};
