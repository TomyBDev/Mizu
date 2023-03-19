#pragma once

class TextureCube
{
public:
	TextureCube(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::wstring fileName);

	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	Gdiplus::Color** pixels;
};
