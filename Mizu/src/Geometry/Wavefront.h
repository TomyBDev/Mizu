#pragma once
#include <Geometry/Mesh.h>

class Wavefront : public Mesh
{
public:
	Wavefront(Microsoft::WRL::ComPtr<ID3D11Device> device, const char* filename);

	void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) override;

private:
	void ReadMaterials(std::unordered_map<std::string, DirectX::XMFLOAT3>& dict, const char* filename);
};