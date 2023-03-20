#include <mzpch.h>
#include <Geometry/CubeMesh.h>

CubeMesh::CubeMesh(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	float zPlus, zMinus, xPlus, xMinus, yPlus, yMinus;
	unsigned long* indices;

	zPlus = 1.f;
	zMinus = -1.f;

	xPlus = 1.f;
	xMinus = -1.f;

	yPlus = 1.f;
	yMinus = -1.f;

	vertexCount = 36;
	indexCount = vertexCount;

	indices = new unsigned long[indexCount];

	using namespace DirectX;

	const Data3D data[] =
	{
		// Top
		{XMFLOAT3(xMinus, yMinus, zMinus), XMFLOAT3(xMinus, yMinus, zMinus),XMFLOAT3(0.0f, yMinus, 0.f)},
		{XMFLOAT3(xMinus, yMinus, zPlus), XMFLOAT3(xMinus, yMinus, zPlus),XMFLOAT3(0.0f, yMinus, 0.f)},
		{XMFLOAT3(xPlus, yMinus, zPlus), XMFLOAT3(xPlus, yMinus, zPlus),XMFLOAT3(0.0f, yMinus, 0.f)},
		{XMFLOAT3(xPlus, yMinus, zMinus), XMFLOAT3(xPlus, yMinus, zMinus),XMFLOAT3(0.0f, yMinus, 0.f)},


		// Bottom
		{XMFLOAT3(xMinus, yPlus, zMinus), XMFLOAT3(xMinus, yPlus, zMinus),XMFLOAT3(0.0f, yPlus, 0.f)},
		{XMFLOAT3(xMinus, yPlus, zPlus), XMFLOAT3(xMinus, yPlus, zPlus),XMFLOAT3(0.0f, yPlus, 0.f)},
		{XMFLOAT3(xPlus, yPlus, zPlus), XMFLOAT3(xPlus, yPlus, zPlus),XMFLOAT3(0.0f, yPlus, 0.f)},
		{XMFLOAT3(xPlus, yPlus, zMinus), XMFLOAT3(xPlus, yPlus, zMinus),XMFLOAT3(0.0f, yPlus, 0.f)},

		// Front
		{XMFLOAT3(xPlus, yMinus, zMinus), XMFLOAT3(xPlus, yMinus, zMinus),XMFLOAT3(xPlus, 0.f, 0.f)},
		{XMFLOAT3(xPlus, yMinus, zPlus), XMFLOAT3(xPlus, yMinus, zPlus),XMFLOAT3(xPlus, 0.f, 0.f)},
		{XMFLOAT3(xPlus, yPlus, zPlus), XMFLOAT3(xPlus, yPlus, zPlus),XMFLOAT3(xPlus, 0.f, 0.f)},
		{XMFLOAT3(xPlus, yPlus, zMinus), XMFLOAT3(xPlus, yPlus, zMinus),XMFLOAT3(xPlus, 0.f, 0.f)},

		// Back
		{XMFLOAT3(xMinus, yMinus, zMinus), XMFLOAT3(xMinus, yMinus, zMinus),XMFLOAT3(xMinus, 0.f, 0.f)},
		{XMFLOAT3(xMinus, yMinus, zPlus), XMFLOAT3(xMinus, yMinus, zPlus),XMFLOAT3(xMinus, 0.f, 0.f)},
		{XMFLOAT3(xMinus, yPlus, zPlus), XMFLOAT3(xMinus, yPlus, zPlus),XMFLOAT3(xMinus, 0.f, 0.f)},
		{XMFLOAT3(xMinus, yPlus, zMinus), XMFLOAT3(xMinus, yPlus, zMinus),XMFLOAT3(xMinus, 0.f, 0.f)},

		// Right
		{XMFLOAT3(xMinus, yMinus, zPlus), XMFLOAT3(xMinus, yMinus, zPlus),XMFLOAT3(0.0f, 0.f, zPlus)},
		{XMFLOAT3(xMinus, yPlus, zPlus), XMFLOAT3(xMinus, yPlus, zPlus),XMFLOAT3(0.0f, 0.f, zPlus)},
		{XMFLOAT3(xPlus, yPlus, zPlus), XMFLOAT3(xPlus, yPlus, zPlus),XMFLOAT3(0.0f, 0.f, zPlus)},
		{XMFLOAT3(xPlus, yMinus, zPlus), XMFLOAT3(xPlus, yMinus, zPlus),XMFLOAT3(0.0f, 0.f, zPlus)},

		// Left
		{XMFLOAT3(xMinus, yMinus, zMinus), XMFLOAT3(xMinus, yMinus, zMinus),XMFLOAT3(0.0f, 0.f, zMinus)},
		{XMFLOAT3(xMinus, yPlus, zMinus), XMFLOAT3(xMinus, yPlus, zMinus),XMFLOAT3(0.0f, 0.f, zMinus)},
		{XMFLOAT3(xPlus, yPlus, zMinus), XMFLOAT3(xPlus, yPlus, zMinus),XMFLOAT3(0.0f, 0.f, zMinus)},
		{XMFLOAT3(xPlus, yMinus, zMinus), XMFLOAT3(xPlus, yMinus, zMinus),XMFLOAT3(0.0f, 0.f, zMinus)},
	};

	for (int i = 0; i < 6; i+=2)
	{
		indices[i * 6 + 0] = i * 4 + 2;  // -, -
		indices[i * 6 + 1] = i * 4 + 1;  // -, +
		indices[i * 6 + 2] = i * 4 + 0;  // +, +

		indices[i * 6 + 3] = i * 4 + 3;	// -, -
		indices[i * 6 + 4] = i * 4 + 2;	// +, +
		indices[i * 6 + 5] = i * 4 + 0;	// +, -
	}

	for (int i = 1; i < 6; i+=2)
	{
		indices[i * 6 + 0] = i * 4 + 0;  // -, -
		indices[i * 6 + 1] = i * 4 + 1;  // -, +
		indices[i * 6 + 2] = i * 4 + 2;  // +, +

		indices[i * 6 + 3] = i * 4 + 0;	// -, -
		indices[i * 6 + 4] = i * 4 + 2;	// +, +
		indices[i * 6 + 5] = i * 4 + 3;	// +, -
	}

	// Set up the description of the vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Data3D) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = data;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now finally create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the index buffer.
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

	delete[] indices;
	indices = 0;
}

void CubeMesh::SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext)
{
	const UINT stride = sizeof(Data3D);
	const UINT offset = 0u;
	deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
