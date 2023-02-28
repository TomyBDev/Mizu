#include "mzpch.h"
#include "Graphics/Texture.h"

Texture::Texture(Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring fileName)
{
	Gdiplus::Bitmap bitmap(fileName.c_str());

	if (bitmap.GetLastStatus() != Gdiplus::Ok)
	{
		LOG_ERROR("Failed to load texture");
		LOG_FLUSH();
		exit(0);
	}

	UINT width = bitmap.GetWidth();
	UINT height = bitmap.GetHeight();
	pixels = new Gdiplus::Color[width * height];

	const BOOL hasAlpha = Gdiplus::IsAlphaPixelFormat(bitmap.GetPixelFormat());

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Gdiplus::Color pixel;
			bitmap.GetPixel(i, j, &pixel);
			if (hasAlpha)
			{
				pixels[(j * width) + i] = { pixel.GetA(), pixel.GetR(), pixel.GetG(),pixel.GetB() };
			}
			else
			{
				pixels[(j * width) + i] = { pixel.GetR(), pixel.GetG(),pixel.GetB() };
			}
		}
	}



	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = (const void*)pixels;
	subResourceData.SysMemPitch = width * sizeof(Gdiplus::Color);
	subResourceData.SysMemSlicePitch = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	HRESULT hr = device->CreateTexture2D(&textureDesc, &subResourceData, &texture2D);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, &textureView);
}

ID3D11ShaderResourceView* Texture::GetShaderResourceView()
{
	return textureView.Get();
}
