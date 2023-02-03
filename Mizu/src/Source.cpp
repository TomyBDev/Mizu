#include "mzpch.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSW windowClass = { 0 };

	HICON hIcon;
	hIcon = (HICON)LoadImage(hInstance, L"Content/mizu.ico", IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);

	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = DefWindowProcW;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = hIcon;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = L"MizuClass";

	RegisterClassW(&windowClass);

	HWND hWnd = CreateWindowExW(0, L"MizuClass", L"Mizu", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 200, 200, 640, 640, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, SW_SHOW);

	while (true);

	return 0;
}

