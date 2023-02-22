#include <mzpch.h>
#include <Graphics/Shaders/NormalShader.h>

NormalShader::NormalShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	if (true)
	{
		LoadVertexShader(L"../bin/Debug-windows-x86_64/Mizu/TriangleShader_vs.cso");
		LoadPixelShader(L"../bin/Debug-windows-x86_64/Mizu/TriangleShader_ps.cso");
	}
	else
	{
		LoadVertexShader(L"TriangleShader_vs.cso");
		LoadPixelShader(L"TriangleShader_ps.cso");
	}

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
}

NormalShader::~NormalShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = NULL;
	}

	if (sampleState)
	{
		sampleState->Release();
		sampleState = NULL;
	}
}

void NormalShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
}