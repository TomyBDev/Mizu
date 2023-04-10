#pragma once
#include <Geometry/Mesh.h>

class MaterialObject : public Mesh
{
public:
	MaterialObject(class Graphics* gfx, const char* filename, class TextureArray** diffTex = nullptr, class TextureArray** bumpTex = nullptr);

	void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;

private:

	struct Material
	{
		DirectX::XMFLOAT3 ambient;
		DirectX::XMFLOAT3 diffuse;
		DirectX::XMFLOAT3 emissive;
		DirectX::XMFLOAT3 specular;
		float specExponent;
		int diffID;
		int bumpID;
		float bumpMult;
	};

	void ReadMaterials(std::unordered_map<std::string, Material>& dict, const char* filename, std::string originalFilename, Graphics* gfx, TextureArray** diffTex = nullptr, TextureArray** bumpTex = nullptr);
};