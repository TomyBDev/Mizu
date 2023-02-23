#include <mzpch.h>
#include <Geometry/PlaneMesh.h>

PlaneMesh::PlaneMesh(Microsoft::WRL::ComPtr<ID3D11Device> device, int width, int height)
{
	indexCount = (width - 1) * (height - 1) * 6;
	vertexCount = indexCount;
	unsigned long* indices = new unsigned long[indexCount];
	Data* data = new Data[vertexCount];

	int index = 0;

	for (int i = 0; i < (width - 1); i++)
	{
		for (int j = 0; j < (height - 1); j++)
		{
			data[index].position = DirectX::XMFLOAT3(i, 0.f, j + 1);
			data[index].normals = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			indices[index] = index;
			index++;

			data[index].position = DirectX::XMFLOAT3(i + 1, 0.f, j + 1);
			data[index].normals = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			indices[index] = index;
			index++;

			data[index].position = DirectX::XMFLOAT3(i, 0.f, j);
			data[index].normals = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			indices[index] = index;
			index++;

			data[index].position = DirectX::XMFLOAT3(i + 1, 0.f, j + 1);
			data[index].normals = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			indices[index] = index;
			index++;

			data[index].position = DirectX::XMFLOAT3(i + 1, 0.f, j);
			data[index].normals = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			indices[index] = index;
			index++;

			data[index].position = DirectX::XMFLOAT3(i, 0.f, j);
			data[index].normals = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
			indices[index] = index;
			index++;
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0u;
	vertexBufferDesc.MiscFlags = 0u;
	vertexBufferDesc.ByteWidth = sizeof(Data) * vertexCount;
	vertexBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = data;

	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);


	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}