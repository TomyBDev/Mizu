#pragma once

class Mesh
{
public:

	struct Vertex
	{
		float x;
		float y;
		float z;
		float w;
	};

	void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	int GetIndexCount();
protected:

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;

	int vertexCount = 0;
	int indexCount = 0;
};