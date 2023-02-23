#pragma once
#include <Geometry/Mesh.h>

class PlaneMesh : public Mesh
{
public:
	PlaneMesh(Microsoft::WRL::ComPtr<ID3D11Device> device, int width = 100, int height = 100);
private:
	int x, y;
};
