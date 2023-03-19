#include "mzpch.h"
#include "Graphics/TextureCube.h"

TextureCube::TextureCube(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::wstring fileName)
{
	Gdiplus::Bitmap bitmap(fileName.c_str());

	if (bitmap.GetLastStatus() != Gdiplus::Ok)
	{
		LOG_ERROR("Failed to load texture");
		LOG_FLUSH();
		exit(0);
	}

	UINT width = bitmap.GetWidth()/6u;
	UINT height = bitmap.GetHeight();
	pixels = new Gdiplus::Color*[6];

	D3D11_SUBRESOURCE_DATA pData[6];
	for (int k = 0; k < 6; k++)
	{
		pixels[k] = new Gdiplus::Color[width * height];

		const BOOL hasAlpha = Gdiplus::IsAlphaPixelFormat(bitmap.GetPixelFormat());

		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				Gdiplus::Color pixel;
				bitmap.GetPixel((k*width) + i, j, &pixel);
				if (hasAlpha)
				{
					pixels[k][(j * width) + i] = { pixel.GetA(), pixel.GetR(), pixel.GetG(),pixel.GetB() };
				}
				else
				{
					pixels[k][(j * width) + i] = { pixel.GetR(), pixel.GetG(),pixel.GetB() };
				}
			}
		}

		//Pointer to the pixel data
		pData[k].pSysMem = pixels[k];
		//Line width in bytes
		pData[k].SysMemPitch = width * sizeof(Gdiplus::Color);
		// This is only used for 3d textures.
		pData[k].SysMemSlicePitch = 0u;
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1; // no mips
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1u;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // | D3D11_BIND_RENDER_TARGET maybe?
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	CHECK_ERROR(device->CreateTexture2D(&textureDesc, pData, &texture2D));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1; // no mips
	CHECK_ERROR(device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, &textureView));
}

ID3D11ShaderResourceView* TextureCube::GetShaderResourceView()
{
	return textureView.Get();
}
