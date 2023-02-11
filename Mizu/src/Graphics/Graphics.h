#pragma once

class Graphics
{
public:
	Graphics(HWND hWnd);
	~Graphics();

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept;
private:
	ID3D11Device* device = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11DeviceContext* deviceContext = nullptr;
	ID3D11RenderTargetView* renderTarget = nullptr;
};