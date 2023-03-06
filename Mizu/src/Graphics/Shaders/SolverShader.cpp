#include <mzpch.h>
#include <Graphics/Shaders/SolverShader.h>

SolverShader::SolverShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadVertexShader(shaderPath L"Shaders/SolverShader_vs.cso");
	LoadPixelShader(shaderPath L"Shaders/SolverShader_ps.cso");

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_BUFFER_DESC scaleBufferDesc;
	scaleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	scaleBufferDesc.ByteWidth = sizeof(ScaleBufferType);
	scaleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	scaleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	scaleBufferDesc.MiscFlags = 0;
	scaleBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&scaleBufferDesc, NULL, &scaleBuffer);

	D3D11_SAMPLER_DESC solverSamplerDesc;
	solverSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	solverSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	solverSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	solverSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	solverSamplerDesc.MipLODBias = 0.0f;
	solverSamplerDesc.MaxAnisotropy = 0u;
	solverSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	solverSamplerDesc.MinLOD = 0;
	solverSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&solverSamplerDesc, &solverSampleState);
}

SolverShader::~SolverShader()
{
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = NULL;
	}

	if (scaleBuffer)
	{
		scaleBuffer->Release();
		scaleBuffer = NULL;
	}

	if (solverSampleState)
	{
		solverSampleState->Release();
		solverSampleState = NULL;
	}
}

void SolverShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* solverTexture)
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

	// Pixel
	ScaleBufferType* scalePtr;
	deviceContext->Map(scaleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	scalePtr = (ScaleBufferType*)mappedResource.pData;
	scalePtr->scale = scale;
	scalePtr->buffer[0] = 0.f;
	scalePtr->buffer[1] = 0.f;
	scalePtr->buffer[2] = 0.f;
	deviceContext->Unmap(scaleBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &scaleBuffer);

	deviceContext->PSSetShaderResources(0, 1, &solverTexture);
	deviceContext->PSSetSamplers(0, 1, &solverSampleState);

	scale = 0.001f;
}