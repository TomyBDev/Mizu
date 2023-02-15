#include <mzpch.h>
#include <Geometry/TriangleMesh.h>

TriangleMesh::TriangleMesh(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	const Vertex vertices[] =
	{
		{0.0f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f},
	};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices;


	device->CreateBuffer(&bufferDesc, &subresourceData, &vertexBuffer);

	indexCount = std::size(vertices);
}
