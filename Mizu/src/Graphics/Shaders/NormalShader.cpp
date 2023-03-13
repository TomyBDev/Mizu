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

	D3D11_SAMPLER_DESC textureSamplerDesc;
	textureSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	textureSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	textureSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	textureSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	textureSamplerDesc.MipLODBias = 0.0f;
	textureSamplerDesc.MaxAnisotropy = 1;
	textureSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	textureSamplerDesc.MinLOD = 0;
	textureSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&textureSamplerDesc, &textureSampleState);
}

NormalShader::~NormalShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = NULL;
	}

	if (textureSampleState)
	{
		textureSampleState->Release();
		textureSampleState = NULL;
	}
}

void NormalShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture)
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

	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &textureSampleState);
}