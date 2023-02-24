#include <mzpch.h>
#include <Graphics/Shaders/SolverShader.h>

SolverShader::SolverShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	if (true)
	{
		LoadVertexShader(L"../bin/Debug-windows-x86_64/Mizu/SolverShader_vs.cso");
		LoadPixelShader(L"../bin/Debug-windows-x86_64/Mizu/SolverShader_ps.cso");
	}
	else
	{
		LoadVertexShader(L"SolverShader_vs.cso");
		LoadPixelShader(L"SolverShader_ps.cso");
	}

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
}

SolverShader::~SolverShader()
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

	if (sampleState)
	{
		sampleState->Release();
		sampleState = NULL;
	}
}

void SolverShader::SetShaderParameters(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, bool bFirstFrame)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	matPtr = (MatrixBufferType*)mappedResource.pData;
	matPtr->world = tworld;
	matPtr->view = tview;
	matPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	DataBufferType* dataPtr;
	deviceContext->Map(dataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (DataBufferType*)mappedResource.pData;
	dataPtr->bFirstFrame = bFirstFrame;
	dataPtr->padding[0] = false;
	dataPtr->padding[1] = false;
	dataPtr->padding[2] = false;
	deviceContext->Unmap(dataBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &dataBuffer);
}