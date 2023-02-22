#include <mzpch.h>
#include <Geometry/TriangleMesh.h>

TriangleMesh::TriangleMesh(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	const Data data[] =
	{
		{DirectX::XMFLOAT3(0.0f, 1.f, 50.f ), DirectX::XMFLOAT3(0.f, 0.f, -1.f )},
		{DirectX::XMFLOAT3(1.f, -1.f, 50.f ), DirectX::XMFLOAT3(0.f, 0.f, -1.f )},
		{DirectX::XMFLOAT3(- 1.f, -1.f, 50.f), DirectX::XMFLOAT3(0.f, 0.f, -1.f)}
	};
	indexCount = std::size(data);
	vertexCount = indexCount;
	unsigned long* indices;
	indices = new unsigned long[indexCount];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0u;
	vertexBufferDesc.MiscFlags = 0u;
	vertexBufferDesc.ByteWidth = sizeof(data);
	vertexBufferDesc.StructureByteStride = sizeof(Data);
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
