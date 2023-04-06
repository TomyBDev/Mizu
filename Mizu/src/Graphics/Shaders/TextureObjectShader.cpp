#include "mzpch.h"
#include "Graphics/Shaders/TextureObjectShader.h"

TextureObjectShader::TextureObjectShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadVertexShader( L"Shaders/TextureObjectShader_vs.cso");
	LoadPixelShader( L"Shaders/TextureObjectShader_ps.cso");

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

	D3D11_SAMPLER_DESC diffuseSamplerDesc;
	diffuseSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	diffuseSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.MipLODBias = 0.0f;
	diffuseSamplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	diffuseSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	diffuseSamplerDesc.MinLOD = 0;
	diffuseSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&diffuseSamplerDesc, &diffuseSampleState);

	D3D11_SAMPLER_DESC normalSamplerDesc;
	normalSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	normalSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	normalSamplerDesc.MipLODBias = 0.0f;
	normalSamplerDesc.MaxAnisotropy = 1;
	normalSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	normalSamplerDesc.MinLOD = 0;
	normalSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&normalSamplerDesc, &normalSampleState);
}

TextureObjectShader::~TextureObjectShader()
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
}

void TextureObjectShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, DirectionalLight dirLight, ID3D11ShaderResourceView* diffuseTex, ID3D11ShaderResourceView* normalTex, ID3D11ShaderResourceView* specularTex)
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

	deviceContext->PSSetShaderResources(0, 1, &diffuseTex);
	deviceContext->PSSetShaderResources(1, 1, &specularTex);
	deviceContext->PSSetSamplers(0, 1, &diffuseSampleState);
	deviceContext->PSSetSamplers(0, 1, &normalSampleState);
}