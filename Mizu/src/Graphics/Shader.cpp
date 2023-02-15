#include <mzpch.h>
#include <Graphics/Shader.h>

using namespace Microsoft::WRL;

Shader::~Shader()
{

	if (vertexShader)
	{
		delete vertexShader;
		vertexShader = nullptr;
	}


	if (pixelShader)
	{
		delete pixelShader;
		pixelShader = nullptr;
	}


	if (inputLayout)
	{
		delete inputLayout;
		inputLayout = nullptr;
	}


	if (matrixBuffer)
	{
		delete matrixBuffer;
		matrixBuffer = nullptr;
	}


	if (sampleState)
	{
		delete sampleState;
		sampleState = nullptr;
	}

}

void Shader::Render(int indexCount)
{

	deviceContext->IASetInputLayout(inputLayout);

	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void Shader::LoadVertexShader(const wchar_t* fileName)
{
	ComPtr<ID3DBlob> blob;
	D3DReadFileToBlob(fileName, &blob);
	device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader);

	const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"Position",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
	};

	device->CreateInputLayout(
		inputElementDesc,
		(UINT)std::size(inputElementDesc),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout);
}

void Shader::LoadPixelShader(const wchar_t* fileName)
{
	ComPtr<ID3DBlob> blob;
	D3DReadFileToBlob(fileName, &blob);
	device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);
}
