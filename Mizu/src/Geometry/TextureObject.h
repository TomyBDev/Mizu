#pragma once
#include <Geometry/Mesh.h>

class TextureObject : public Mesh
{
public:
	TextureObject(Microsoft::WRL::ComPtr<ID3D11Device> device, const char* filename);

	void SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override;
};