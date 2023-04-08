#include "mzpch.h"
#include "Graphics/Shaders/WaterShader.h"
#include <Graphics/Camera.h>

WaterShader::WaterShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadVertexShader( L"Shaders/WaterShader_vs.cso");
	LoadPixelShader( L"Shaders/WaterShader_ps.cso");
	LoadHullShader( L"Shaders/WaterShader_hs.cso");
	LoadDomainShader( L"Shaders/WaterShader_ds.cso");

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(LightBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	D3D11_BUFFER_DESC controlBufferDesc;
	controlBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	controlBufferDesc.ByteWidth = sizeof(ControlBufferType);
	controlBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	controlBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	controlBufferDesc.MiscFlags = 0;
	controlBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&controlBufferDesc, NULL, &controlBuffer);

	D3D11_BUFFER_DESC tessellationBufferDesc;
	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&tessellationBufferDesc, NULL, &tessellationBuffer);

	D3D11_BUFFER_DESC resolutionBufferDesc;
	resolutionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	resolutionBufferDesc.ByteWidth = sizeof(ResolutionBufferType);
	resolutionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	resolutionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	resolutionBufferDesc.MiscFlags = 0;
	resolutionBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&resolutionBufferDesc, NULL, &resolutionBuffer);

	D3D11_SAMPLER_DESC skySamplerDesc;
	skySamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	skySamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	skySamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	skySamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	skySamplerDesc.MipLODBias = 0.0f;
	skySamplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	skySamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	skySamplerDesc.MinLOD = 0;
	skySamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&skySamplerDesc, &skySampleState);

	/*D3D11_SAMPLER_DESC heightMapSamplerDesc;
	heightMapSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	heightMapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.MipLODBias = 0.0f;
	heightMapSamplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	heightMapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	heightMapSamplerDesc.MinLOD = 0;
	heightMapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&heightMapSamplerDesc, &heightMapSampleState);*/

	D3D11_SAMPLER_DESC heightMapSamplerDesc;
	heightMapSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	heightMapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	heightMapSamplerDesc.MipLODBias = 0.0f;
	heightMapSamplerDesc.MaxAnisotropy = 1;
	heightMapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	heightMapSamplerDesc.MinLOD = 0;
	heightMapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&heightMapSamplerDesc, &heightMapSampleState);
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

	if (skySampleState)
	{
		skySampleState->Release();
		skySampleState = NULL;
	}

	if (heightMapSampleState)
	{
		heightMapSampleState->Release();
		heightMapSampleState = NULL;
	}
}

void WaterShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMapTexture, ID3D11ShaderResourceView* skyTextureCube, DirectionalLight dirLight, Camera* camera, float* shallowColor, float* deepColor, float strength, int res)
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

	ResolutionBufferType* resPtr;
	deviceContext->Map(resolutionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	resPtr = (ResolutionBufferType*)mappedResource.pData;
	resPtr->res = res;
	resPtr->buffer = {0.f, 0.f ,0.f};
	deviceContext->Unmap(resolutionBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &resolutionBuffer);

	deviceContext->VSSetShaderResources(0, 1, &heightMapTexture);
	deviceContext->VSSetSamplers(0, 1, &heightMapSampleState);

	// Pixel
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient = dirLight.ambient;
	lightPtr->diffuse = dirLight.diffuse;
	lightPtr->direction = dirLight.direction;
	lightPtr->buffer = 0.f;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->position = { camera->GetPosition().m128_f32[0],camera->GetPosition().m128_f32[1],camera->GetPosition().m128_f32[2] };
	cameraPtr->direction = { camera->GetForward().m128_f32[0],camera->GetForward().m128_f32[1],camera->GetForward().m128_f32[2] };
	cameraPtr->buffer = { 0.f, 0.f };
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &cameraBuffer);

	ControlBufferType* controlPtr;
	deviceContext->Map(controlBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	controlPtr = (ControlBufferType*)mappedResource.pData;
	controlPtr->strength = strength;
	controlPtr->shallowColor = { shallowColor[0],shallowColor[1],shallowColor[2],shallowColor[3] };
	controlPtr->deepColor = { deepColor[0],deepColor[1],deepColor[2],deepColor[3] };
	controlPtr->buffer = { 0.f, 0.f, 0.f };
	deviceContext->Unmap(controlBuffer, 0);
	deviceContext->PSSetConstantBuffers(2, 1, &controlBuffer);

	deviceContext->PSSetConstantBuffers(3, 1, &matrixBuffer);

	deviceContext->PSSetShaderResources(0, 1, &heightMapTexture);
	deviceContext->PSSetShaderResources(1, 1, &skyTextureCube);
	deviceContext->PSSetSamplers(0, 1, &skySampleState);

	// Hull
	TessellationBufferType* tessPtr;
	deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessellationBufferType*)mappedResource.pData;
	tessPtr->first = tessellationTable[res];
	tessPtr->second = tessellationTable[res];
	tessPtr->buffer = {0.f, 0.f};
	deviceContext->Unmap(tessellationBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessellationBuffer);

	// Domain
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->DSSetShaderResources(0, 1, &heightMapTexture);
	deviceContext->DSSetSamplers(0, 1, &heightMapSampleState);
}