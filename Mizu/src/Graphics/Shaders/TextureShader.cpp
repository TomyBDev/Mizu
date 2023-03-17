#include <mzpch.h>
#include <Graphics/Shaders/TextureShader.h>

TextureShader::TextureShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadTextureVertexShader(shaderPath L"Shaders/TextureShader_vs.cso");
	LoadPixelShader(shaderPath L"Shaders/TextureShader_ps.cso");

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

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
	device->CreateSamplerState(&copySamplerDesc, &sampleState);
}

TextureShader::~TextureShader()
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

void TextureShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture)
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

	// Pixel Shader
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}