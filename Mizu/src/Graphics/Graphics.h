#pragma once

class Graphics
{
public:
	Graphics(HWND hwnd);

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return device; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() { return deviceContext; }

	const DirectX::XMMATRIX GetWorldMatrix() { return worldMatrix; };
	const DirectX::XMMATRIX GetProjectionMatrix() { return projectionMatrix; };
	const DirectX::XMMATRIX GetOrthoMatrix() { return orthoMatrix; };

	HWND GetHWND() { return hWnd; }

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
	void SetZBuffer(bool b);
	void SetAlpha(bool b);
	void SetBackBufferRenderTarget();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> noDepthStencilState = nullptr;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState = nullptr;
	Microsoft::WRL::ComPtr<ID3D11BlendState> noBlendState = nullptr;
	D3D11_VIEWPORT viewport;
	HWND hWnd;

	DirectX::XMMATRIX worldMatrix, projectionMatrix, orthoMatrix;
};