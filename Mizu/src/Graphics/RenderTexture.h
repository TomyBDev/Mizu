#pragma once

class RenderTexture
{
public:
	RenderTexture(ID3D11Device* device, int texWidth, int texHeight, float screenNear, float screenFar);
	~RenderTexture();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext);
	void clearRenderTarget(ID3D11DeviceContext* deviceContext, float r, float g, float b);

	ID3D11ShaderResourceView* GetShaderResourceView();
};
