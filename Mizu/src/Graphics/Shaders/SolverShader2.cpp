#include <mzpch.h>
#include <Graphics/Shaders/SolverShader2.h>

SolverShader2::SolverShader2(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadVertexShader(shaderPath L"Shaders/SolverShader2_vs.cso");
	LoadPixelShader(shaderPath L"Shaders/SolverShader2_ps.cso");

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

	D3D11_SAMPLER_DESC solverSamplerDesc;
	solverSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	solverSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	solverSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	solverSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	solverSamplerDesc.MipLODBias = 0.0f;
	solverSamplerDesc.MaxAnisotropy = 1;
	solverSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	solverSamplerDesc.MinLOD = 0;
	solverSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&solverSamplerDesc, &solverSampleState);
}

SolverShader2::~SolverShader2()
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

	if (solverSampleState)
	{
		solverSampleState->Release();
		solverSampleState = NULL;
	}
}

void SolverShader2::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* oldTexture, ID3D11ShaderResourceView* pass1Texture, float dt, int resolution)
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
	DataBufferType* dataPtr;
	deviceContext->Map(dataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (DataBufferType*)mappedResource.pData;
	dataPtr->dt = dt;
	dataPtr->res = resolution;
	dataPtr->buffer = { 0.f, 0.f };
	deviceContext->Unmap(dataBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &dataBuffer);

	deviceContext->PSSetShaderResources(0, 1, &oldTexture);
	deviceContext->PSSetShaderResources(1, 1, &pass1Texture);
	deviceContext->PSSetSamplers(0, 1, &solverSampleState);
}