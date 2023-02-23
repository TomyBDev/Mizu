#pragma once

class Mesh
{
public:

	struct Data
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normals;
	};

	void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	int GetIndexCount();
protected:

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;

	int vertexCount = 0;
	int indexCount = 0;
};