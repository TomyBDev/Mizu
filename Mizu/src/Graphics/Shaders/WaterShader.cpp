#include "mzpch.h"
#include "Graphics/Shaders/WaterShader.h"

WaterShader::WaterShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadVertexShader(shaderPath L"Shaders/WaterShader_vs.cso");
	LoadPixelShader(shaderPath L"Shaders/WaterShader_ps.cso");

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_SAMPLER_DESC heightMapSamplerDesc;
	heightMapSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	heightMapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.MipLODBias = 0.0f;
	heightMapSamplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	heightMapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	heightMapSamplerDesc.MinLOD = 0;
	heightMapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&heightMapSamplerDesc, &heightMapSampleState);

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

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

WaterShader::~WaterShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = NULL;
	}

	if (heightMapSampleState)
	{
		heightMapSampleState->Release();
		heightMapSampleState = NULL;
	}

	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = NULL;
	}

	if (waterSampleState)
	{
		waterSampleState->Release();
		waterSampleState = NULL;
	}
}

void WaterShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMapTexture, ID3D11ShaderResourceView* waterTexture, DirectionalLight dirLight, int textureMode)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Vertex
	MatrixBufferType* matPtr;
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	matPtr = (MatrixBufferType*)mappedResource.pData;
	matPtr->world = tworld;
	matPtr->view = tview;
	matPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->VSSetShaderResources(0, 1, &heightMapTexture);
	deviceContext->VSSetSamplers(0, 1, &heightMapSampleState);

	// Pixel

	// Vertex
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient = dirLight.ambient;
	lightPtr->diffuse = dirLight.diffuse;
	lightPtr->direction = dirLight.direction;
	lightPtr->textureMode = textureMode;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	if (textureMode)
		deviceContext->PSSetShaderResources(0, 1, &heightMapTexture);
	else
		deviceContext->PSSetShaderResources(0, 1, &waterTexture);
	deviceContext->PSSetSamplers(0, 1, &waterSampleState);
}