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

	struct Data3D
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 texture;
		DirectX::XMFLOAT3 normals;
	};

	struct TextureData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normals;
	};

	struct MaterialData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normals;
		DirectX::XMFLOAT3 materials;
	};

	virtual void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	int GetIndexCount();
protected:

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;

	int vertexCount = 0;
	int indexCount = 0;
};