#include "mzpch.h"
#include "Graphics/Shaders/MaterialObjectShader.h"

MaterialObjectShader::MaterialObjectShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadMaterialVertexShader( L"Shaders/MaterialObjectShader_vs.cso");
	LoadPixelShader( L"Shaders/MaterialObjectShader_ps.cso");

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

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
}

MaterialObjectShader::~MaterialObjectShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = NULL;
	}

	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = NULL;
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

	if (cameraBuffer)
	{
		cameraBuffer->Release();
		cameraBuffer = NULL;
	}
}

void MaterialObjectShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, DirectionalLight dirLight, ID3D11ShaderResourceView* diffSrv, ID3D11ShaderResourceView* normSrv, XMVECTOR camPos)
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

	CameraBufferType* camPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->camPos =  { camPos.m128_f32[0],camPos.m128_f32[1],camPos.m128_f32[2] };
	camPtr->padding = 0.f;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &cameraBuffer);

	deviceContext->PSSetShaderResources(0, 1, &diffSrv);
	deviceContext->PSSetShaderResources(1, 1, &normSrv);
	deviceContext->PSSetSamplers(0, 1, &normalSampleState);
	deviceContext->PSSetSamplers(1, 1, &copySampleState);
}