#include <mzpch.h>
#include <Geometry/Mesh.h>

void Mesh::SendData(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext)
{
	const UINT stride = sizeof(DirectX::XMFLOAT3);
	const UINT offset = 0u;
	deviceContext->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int Mesh::GetIndexCount()
{
	return indexCount;
}
