#pragma once

class Graphics
{
public:
	Graphics(HWND hWnd);

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
	void DrawTriangle();
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTarget = nullptr;
};