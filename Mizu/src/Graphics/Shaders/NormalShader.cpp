#include <mzpch.h>
#include <Graphics/Shaders/NormalShader.h>

NormalShader::NormalShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadVertexShader(shaderPath L"Shaders/NormalShader_vs.cso");
	LoadPixelShader(shaderPath L"Shaders/NormalShader_ps.cso");

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_BUFFER_DESC dataBufferDesc;
	dataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	dataBufferDesc.ByteWidth = sizeof(DataBufferType);
	dataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dataBufferDesc.MiscFlags = 0;
	dataBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&dataBufferDesc, NULL, &dataBuffer);

	D3D11_SAMPLER_DESC normalSamplerDesc;
	normalSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	normalSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.MipLODBias = 0.0f;
	normalSamplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	normalSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	normalSamplerDesc.MinLOD = 0;
	normalSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&normalSamplerDesc, &normalSampleState);

	D3D11_SAMPLER_DESC copySamplerDesc;
	copySamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	copySamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	copySamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	copySamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	copySamplerDesc.MipLODBias = 0.0f;
	copySamplerDesc.MaxAnisotropy = 1;
	copySamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	copySamplerDesc.MinLOD = 0;
	copySamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&copySamplerDesc, &copySampleState);
}

NormalShader::~NormalShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = NULL;
	}

	if (dataBuffer)
	{
		dataBuffer->Release();
		dataBuffer = NULL;
	}

	if (normalSampleState)
	{
		normalSampleState->Release();
		normalSampleState = NULL;
	}

	if (copySampleState)
	{
		copySampleState->Release();
		copySampleState = NULL;
	}
}

void NormalShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float uvScale, bool bCopyMode)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Vertex Shader
	MatrixBufferType* matrixPtr;
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	matrixPtr = (MatrixBufferType*)mappedResource.pData;
	matrixPtr->world = tworld;
	matrixPtr->view = tview;
	matrixPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	//Pixel Shader
	DataBufferType* dataPtr;
	deviceContext->Map(dataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (DataBufferType*)mappedResource.pData;
	dataPtr->uvScale = uvScale;
	dataPtr->copyMode = bCopyMode;
	dataPtr->buffer1[0] = false;
	dataPtr->buffer1[1] = false;
	dataPtr->buffer1[2] = false;
	dataPtr->buffer2 = {0.f, 0.f};
	deviceContext->Unmap(dataBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &dataBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &normalSampleState);
	deviceContext->PSSetSamplers(1, 1, &copySampleState);
}