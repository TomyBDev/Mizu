#include <mzpch.h>
#include <Geometry/OrthoMesh.h>

OrthoMesh::OrthoMesh(Microsoft::WRL::ComPtr<ID3D11Device> device, int width, int height, int xPos, int yPos)
{
	float left, right, top, bottom;
	unsigned long* indices;

	// Calculate the screen coordinates of the left side of the window.
	left = static_cast<float>(xPos) - (static_cast<float>(width) / 2);

	// Calculate the screen coordinates of the right side of the window.
	right = left + static_cast<float>(width);

	// Calculate the screen coordinates of the top of the window.
	top = (static_cast<float>(height) / 2) + yPos;

	// Calculate the screen coordinates of the bottom of the window.
	bottom = top - static_cast<float>(height);

	vertexCount = 6;
	indexCount = vertexCount;

	indices = new unsigned long[indexCount];

	using namespace DirectX;

	const Data data[] =
	{
		{XMFLOAT3(left, bottom, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)},
		{XMFLOAT3(left, top, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)},
		{XMFLOAT3(right, top, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)},
		{XMFLOAT3(right, bottom, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}
	};

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 2;  // Top right.
	indices[2] = 1;  // Top left.

	indices[3] = 0;	// bottom left
	indices[4] = 3;	// bottom right
	indices[5] = 2;	// top right

	// Set up the description of the vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Data) * vertexCount;
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