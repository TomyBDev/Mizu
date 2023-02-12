#include <mzpch.h>
#include <Graphics/Graphics.h>

using namespace Microsoft::WRL;

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
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
}

void Graphics::EndFrame()
{
	swapChain->Present(1u, 0u);
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r, g, b, 1.0f };
	deviceContext->ClearRenderTargetView(renderTarget.Get(), color);
}

void Graphics::DrawTriangle()
{
	struct Vertex
	{
		float x;
		float y;
	};

	const Vertex vertices[] =
	{
		{0.0f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f},
	};


	ComPtr<ID3D11Buffer> vertexBuffer;
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices;


	device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);

	//Pixel

	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3DBlob> blob;
	D3DReadFileToBlob(L"../bin/Debug-windows-x86_64/Mizu/ps_TriangleShader.cso", &blob);
	device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);

	deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0u);

	//Vertex

	ComPtr<ID3D11VertexShader> vertexShader;
	D3DReadFileToBlob(L"../bin/Debug-windows-x86_64/Mizu/vs_TriangleShader.cso", &blob);
	device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader);

	deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0u);

	ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"Position",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
	};
	device->CreateInputLayout(
		inputElementDesc,
		(UINT)std::size(inputElementDesc),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout);

	deviceContext->IASetInputLayout(inputLayout.Get());

	deviceContext->OMSetRenderTargets(1u, renderTarget.GetAddressOf(), nullptr);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT viewport;
	viewport.Width = 1280;
	viewport.Height = 720;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	deviceContext->RSSetViewports(1u, &viewport);

	deviceContext->Draw(std::size(vertices), 0u);

}