#pragma once
#include <Geometry/Mesh.h>

class OrthoMesh : public Mesh
{
public:
	OrthoMesh(Microsoft::WRL::ComPtr<ID3D11Device> device, int width = 100, int height = 100, int xPos = 0, int yPos = 0);

	void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext) override;
};
