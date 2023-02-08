#pragma once

class RenderWindow
{
public:
	RenderWindow(HINSTANCE hInst)
		: hInstance(hInst) {}

	~RenderWindow();

	void Initialize();

	BOOL Update();

	void CreateRenderWindow();
	void CreateWinClass();

private:
	HINSTANCE hInstance;
	MSG message;
	BOOL gResult;
};