#include "mzpch.h"
#include "Graphics/RenderTexture.h"

RenderTexture::RenderTexture(ID3D11Device* device, int texWidth, int texHeight, float screenNear, float screenFar)
{
}

RenderTexture::~RenderTexture()
{
}

void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
}

void RenderTexture::clearRenderTarget(ID3D11DeviceContext* deviceContext, float r, float g, float b)
{
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView()
{
	return nullptr;
}
