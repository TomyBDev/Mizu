#include <mzpch.h>
#include <Geometry/Mesh.h>

void Mesh::SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, D3D_PRIMITIVE_TOPOLOGY topology)
{
	const UINT stride = sizeof(Data);
	const UINT offset = 0u;
	deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(topology);
}

int Mesh::GetIndexCount()
{
	return indexCount;
}
