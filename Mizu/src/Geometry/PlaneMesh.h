#pragma once
#include <Geometry/Mesh.h>

class PlaneMesh : public Mesh
{
public:
	PlaneMesh(Microsoft::WRL::ComPtr<ID3D11Device> device);
};
