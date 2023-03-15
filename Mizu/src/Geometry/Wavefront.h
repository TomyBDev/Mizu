#pragma once
#include <Geometry/Mesh.h>

class Wavefront : public Mesh
{
public:
	Wavefront(Microsoft::WRL::ComPtr<ID3D11Device> device, const char* filename);
};