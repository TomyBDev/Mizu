#include <mzpch.h>
#include <Graphics/Graphics.h>

using namespace Microsoft::WRL;

Graphics::Graphics(HWND hwnd)
{
	hWnd = hwnd;

	const UINT width = 1280u;
	const UINT height = 720u;

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0u;
	sd.BufferDesc.Height = 0u;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0u;
	sd.BufferDesc.RefreshRate.Denominator = 0u;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 4u;
	sd.SampleDesc.Quality = 0u;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1u;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0u;

	CHECK_ERROR(D3D11CreateDeviceAndSwapChain(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr, 
		0u,
		nullptr, 
		0, 
		D3D11_SDK_VERSION, 
		&sd, 
		&swapChain, 
		&device, 
		nullptr, 
		&deviceContext));

	ComPtr<ID3D11Resource> backBuffer = nullptr;
	CHECK_ERROR(swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer));
	CHECK_ERROR(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTarget));

	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	//(0 == no vsync)
	CHECK_ERROR(swapChain->Present(0, 0));

	worldMatrix = DirectX::XMMatrixIdentity();

	// Create the projection matrix for 3D rendering.
	projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(3.1415f/2.f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 200.f);

	// Create the ortho matrix for 2D Rendering.
	orthoMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), 0.1f, 200.f);

	// Create depth stencil
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0u;
	depthStencilDesc.StencilWriteMask = 0u;
	CHECK_ERROR(device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState));

	// Create no depth stencil
	D3D11_DEPTH_STENCIL_DESC noDepthStencilDesc = depthStencilDesc;
	noDepthStencilDesc.DepthEnable = false;
	CHECK_ERROR(device->CreateDepthStencilState(&noDepthStencilDesc, &noDepthStencilState));

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
	CHECK_ERROR(device->CreateTexture2D(&depthDesc, nullptr, &depthStencilTexture));

	// Create Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthStencilViewDesc.Texture2D.MipSlice = 0u;
	CHECK_ERROR(device->CreateDepthStencilView(depthStencilTexture.Get(), &depthStencilViewDesc, &depthStencilView));


	// Bind depth stencil view
	deviceContext->OMSetRenderTargets(1u, renderTarget.GetAddressOf(), depthStencilView.Get());
	deviceContext->RSSetViewports(1u, &viewport);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	//rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = TRUE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;

	CHECK_ERROR(device->CreateRasterizerState(&rasterizerDesc, &rasterizerState));
	deviceContext->RSSetState(rasterizerState.Get());

	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	CHECK_ERROR(device->CreateRasterizerState(&rasterizerDesc, &noBackCullRasterizerState));

	//// Blender

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	CHECK_ERROR(device->CreateBlendState(&blendDesc, &blendState));

	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);
}

void Graphics::EndFrame()
{
	CHECK_ERROR(swapChain->Present(1u, 0u));
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

void Graphics::SetAlpha(bool b)
{
	if (b)
	{
		deviceContext->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFFu);
		return;
	}

	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFFu);
}

void Graphics::SetBothSides(bool b)
{
	if (b)
	{
		deviceContext->RSSetState(noBackCullRasterizerState.Get());
		return;
	}

	deviceContext->RSSetState(rasterizerState.Get());
}

void Graphics::SetBackBufferRenderTarget()
{
	deviceContext->OMSetRenderTargets(1u, renderTarget.GetAddressOf(), depthStencilView.Get());
	deviceContext->RSSetViewports(1, &viewport);
}
