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
		DirectX::XMFLOAT3 ambient;
		DirectX::XMFLOAT3 diffuse;
		DirectX::XMFLOAT3 emissive;
		DirectX::XMFLOAT3 specular;
		float specExponent;
		int diffID;
		int bumpID;
		float bumpMult;
	};

	virtual void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	int GetIndexCount();
protected:

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;

	int vertexCount = 0;
	int indexCount = 0;
};