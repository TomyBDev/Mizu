#pragma once
#include <Geometry/Mesh.h>

class CubeMesh : public Mesh
{
public:
	CubeMesh(Microsoft::WRL::ComPtr<ID3D11Device> device);

	void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;
};
