#include "mzpch.h"
#include "Graphics/TextureArray.h"

TextureArray::TextureArray(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::vector<std::wstring>* fileNames)
{
	std::wstring filePrefix =  L"Content/Textures/";
	size = fileNames->size();

	pixels = new Gdiplus::Color*[size];
	D3D11_SUBRESOURCE_DATA* pData = new D3D11_SUBRESOURCE_DATA[size];

	UINT width;
	UINT height;

	for (int k = 0; k < size; k++)
	{
		//Format Filename
		std::wstring combined = filePrefix;
		combined.append(fileNames->at(k));
		Gdiplus::Bitmap bitmap(combined.c_str());

		if (bitmap.GetLastStatus() != Gdiplus::Ok)
		{
			LOG_ERROR("Failed to load texture");
			LOG_FLUSH();
			exit(0);
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();

		pixels[k] = new Gdiplus::Color[width * height];

		const BOOL hasAlpha = Gdiplus::IsAlphaPixelFormat(bitmap.GetPixelFormat());

		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < width; i++)
			{
				Gdiplus::Color pixel;
				bitmap.GetPixel(i, j, &pixel);
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
	textureDesc.MipLevels = 1; // all the mips >:D
	textureDesc.ArraySize = size;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1u;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // | D3D11_BIND_RENDER_TARGET maybe?
	textureDesc.CPUAccessFlags = 0u;
	textureDesc.MiscFlags = 0u;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	CHECK_ERROR(device->CreateTexture2D(&textureDesc, pData, &texture2D));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2DArray.MipLevels = textureDesc.MipLevels; // no mips
	shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
	shaderResourceViewDesc.Texture2DArray.ArraySize = size;
	CHECK_ERROR(device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, &textureView));
}

ID3D11ShaderResourceView* TextureArray::GetShaderResourceView()
{
	return textureView.Get();
}
