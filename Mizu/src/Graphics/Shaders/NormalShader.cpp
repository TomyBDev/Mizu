#include <mzpch.h>
#include <Graphics/Shaders/NormalShader.h>

NormalShader::NormalShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadVertexShader(shaderPath L"Shaders/TriangleShader_vs.cso");
	LoadPixelShader(shaderPath L"Shaders/TriangleShader_ps.cso");

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_BUFFER_DESC timeBufferDesc;
	timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	timeBufferDesc.ByteWidth = sizeof(TimeBufferType);
	timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	timeBufferDesc.MiscFlags = 0;
	timeBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&timeBufferDesc, NULL, &timeBuffer);

	D3D11_SAMPLER_DESC waterSamplerDesc;
	waterSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	waterSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	waterSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	waterSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	waterSamplerDesc.MipLODBias = 0.0f;
	waterSamplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	waterSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	waterSamplerDesc.MinLOD = 0;
	waterSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&waterSamplerDesc, &waterSampleState);
}

NormalShader::~NormalShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = NULL;
	}

	if (timeBuffer)
	{
		timeBuffer->Release();
		timeBuffer = NULL;
	}

	if (waterSampleState)
	{
		waterSampleState->Release();
		waterSampleState = NULL;
	}
}

void NormalShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* waterTexture, float timeElapsed)
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

	TimeBufferType* timePtr;
	deviceContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	timePtr = (TimeBufferType*)mappedResource.pData;
	timePtr->time = timeElapsed;
	timePtr->padding = { 0,0,0 };
	deviceContext->Unmap(timeBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &timeBuffer);

	deviceContext->PSSetShaderResources(0, 1, &waterTexture);
	deviceContext->PSSetSamplers(0, 1, &waterSampleState);
}