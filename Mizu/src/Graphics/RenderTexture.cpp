#include "mzpch.h"
#include "Graphics/RenderTexture.h"

RenderTexture::RenderTexture(Microsoft::WRL::ComPtr<ID3D11Device> device, int texWidth, int texHeight, float screenNear, float screenFar)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texWidth;
	textureDesc.Height = texHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1u;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
	device->CreateTexture2D(&textureDesc, nullptr, &texture2D);

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1; // use all mip levels
	device->CreateShaderResourceView(texture2D.Get(), &shaderResourceViewDesc, &textureView);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	HRESULT hr = device->CreateRenderTargetView(texture2D.Get(), &renderTargetViewDesc, &renderTargetView);


	viewport.Width = static_cast<float>(texWidth);
	viewport.Height = static_cast<float>(texHeight);
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;
	viewport.TopLeftX = 0.f;
	viewport.TopLeftY = 0.f;

	// Create the projection matrix for 3D rendering.
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(3.1415f / 2.f, static_cast<float>(texWidth) / static_cast<float>(texHeight), screenNear, screenFar);

	// Create the ortho matrix for 2D Rendering.
	orthoMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(texWidth), static_cast<float>(texHeight), screenNear, screenFar);

	// Create depth stencil
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	// Create depth Stencil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture;
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = texWidth;
	depthDesc.Height = texHeight;
	depthDesc.MipLevels = 1u;
	depthDesc.ArraySize = 1u;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1u;
	depthDesc.SampleDesc.Quality = 0u;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	device->CreateTexture2D(&depthDesc, nullptr, &depthStencilTexture);

	// Create Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(depthStencilTexture.Get(), &depthStencilViewDesc, &depthStencilView);
}

RenderTexture::~RenderTexture()
{
}

void RenderTexture::SetRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext)
{
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView.Get());
	deviceContext->RSSetViewports(1, &viewport);
}

void RenderTexture::ClearRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, float r, float g, float b)
{
	const float color[] = { r, g, b, 1.0f };
	LOG_ERROR(StringConverter::GetLastErrorAsString());
	LOG_ERROR("Hello");
	LOG_FLUSH();
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView()
{
	return textureView.Get();
}
