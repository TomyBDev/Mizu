#pragma once
#include <Geometry/Mesh.h>

class MaterialObject : public Mesh
{
public:
	MaterialObject(Microsoft::WRL::ComPtr<ID3D11Device> device, const char* filename);

	void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;

private:
	void ReadMaterials(std::unordered_map<std::string, DirectX::XMFLOAT3>& dict, const char* filename, std::string originalFilename);
};