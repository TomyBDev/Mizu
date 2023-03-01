#pragma once

class RenderTexture
{
public:
	RenderTexture(Microsoft::WRL::ComPtr<ID3D11Device> device, int texWidth, int texHeight, float screenNear, float screenFar);
	~RenderTexture();

	void SetRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	void ClearRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, float r, float g, float b);

	ID3D11ShaderResourceView* GetShaderResourceView();

	const DirectX::XMMATRIX GetProjectionMatrix() { return projectionMatrix; };
	const DirectX::XMMATRIX GetOrthoMatrix() { return orthoMatrix; };

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState = nullptr;
	D3D11_VIEWPORT viewport;
	DirectX::XMMATRIX worldMatrix, projectionMatrix, orthoMatrix;
};
