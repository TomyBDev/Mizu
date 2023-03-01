#include <mzpch.h>
#include <Graphics/Graphics.h>

using namespace Microsoft::WRL;

Graphics::Graphics(HWND hwnd)
{
	hWnd = hwnd;

	const UINT width = 1280u;
	const UINT height = 720u;

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 4u;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		0,
		nullptr, 
		0, 
		D3D11_SDK_VERSION, 
		&sd, 
		&swapChain, 
		&device, 
		nullptr, 
		&deviceContext);

	ComPtr<ID3D11Resource> backBuffer = nullptr;
	swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);
	device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTarget);
	backBuffer->Release();

	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	//(0 == no vsync)
	swapChain->Present(0, 0);

	worldMatrix = DirectX::XMMatrixIdentity();

	// Create the projection matrix for 3D rendering.
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(3.1415f/2.f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 200.f);

	// Create the ortho matrix for 2D Rendering.
	orthoMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), 0.1f, 200.f);

	// Create depth stencil
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

	// Create no depth stencil
	D3D11_DEPTH_STENCIL_DESC noDepthStencilDesc;
	noDepthStencilDesc.DepthEnable = false;
	noDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&depthStencilDesc, &noDepthStencilState);

	// Bind depth state
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1u);

	// Create depth Stencil texture
	ComPtr<ID3D11Texture2D> depthStencilTexture;
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = width;
	depthDesc.Height = height;
	depthDesc.MipLevels = 1u;
	depthDesc.ArraySize = 1u;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.SampleDesc.Count = 4u;
	depthDesc.SampleDesc.Quality = 0u;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	device->CreateTexture2D(&depthDesc, nullptr, &depthStencilTexture);

	// Create Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(depthStencilTexture.Get(), &depthStencilViewDesc, &depthStencilView);

	// Bind depth stencil view
	deviceContext->OMSetRenderTargets(1u, renderTarget.GetAddressOf(), depthStencilView.Get());
	deviceContext->RSSetViewports(1u, &viewport);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = TRUE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;

	ComPtr<ID3D11RasterizerState> rasterizerState;
	device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	deviceContext->RSSetState(rasterizerState.Get());
}

void Graphics::EndFrame()
{
	swapChain->Present(1u, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r, g, b, 1.0f };
	deviceContext->ClearRenderTargetView(renderTarget.Get(), color);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
}

void Graphics::SetZBuffer(bool b)
{
	if (b)
	{
		deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 1u);
		return;
	}

	deviceContext->OMSetDepthStencilState(noDepthStencilState.Get(), 1u);
	
}

void Graphics::SetBackBufferRenderTarget()
{
	deviceContext->OMSetRenderTargets(1u, renderTarget.GetAddressOf(), depthStencilView.Get());
}
