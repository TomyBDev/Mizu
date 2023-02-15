#include <mzpch.h>
#include <Graphics/Shaders/NormalShader.h>

NormalShader::NormalShader(Microsoft::WRL::ComPtr<ID3D11Device> dev, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) : Shader(dev, context)
{
	LoadVertexShader(L"../bin/Debug-windows-x86_64/Mizu/TriangleShader_vs.cso");
	LoadPixelShader(L"../bin/Debug-windows-x86_64/Mizu/TriangleShader_ps.cso");
}
