#pragma once

class Graphics
{
public:
	Graphics(HWND hwnd);

	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return device; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() { return deviceContext; }

	const DirectX::XMMATRIX GetWorldMatrix() { return worldMatrix; };
	const DirectX::XMMATRIX GetProjectionMatrix() { return projectionMatrix; };

	HWND GetHWND() { return hWnd; }

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
	void SetRenderTarget();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget = nullptr;
	D3D11_VIEWPORT viewport;
	HWND hWnd;

	DirectX::XMMATRIX worldMatrix, projectionMatrix;
};