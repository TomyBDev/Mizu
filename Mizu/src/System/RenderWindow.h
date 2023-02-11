#pragma once

class InputManager;

class RenderWindow
{
public:
	RenderWindow(HINSTANCE hInst, int w = 1280, int h = 720, std::wstring winName = L"Mizu")
		: hInstance(hInst),
		width(w),
		height(h),
		windowName(winName) {}

	~RenderWindow();

	void Initialize(InputManager* input);

	void WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool Update();

	void CreateRenderWindow();
	void CreateWinClass();

	class Graphics* GetGraphics() { return graphics; }

private:
	HINSTANCE hInstance;
	MSG message;
	BOOL gResult;

	int width;
	int height;
	std::wstring windowName;

	InputManager* inputManager;
	Graphics* graphics;
};