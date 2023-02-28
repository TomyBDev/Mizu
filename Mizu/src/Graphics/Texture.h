#pragma once

class Texture
{
public:
	Texture(Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring fileName);

	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	Gdiplus::Color* pixels;
};
