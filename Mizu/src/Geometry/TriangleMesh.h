#pragma once
#include <Geometry/Mesh.h>

class TriangleMesh : public Mesh
{
public:
	TriangleMesh(Microsoft::WRL::ComPtr<ID3D11Device> device);
};