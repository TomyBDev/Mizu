#include <mzpch.h>
#include <System/RenderWindow.h>

//extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;

	case WM_KEYDOWN:
		if (wParam == 'F')
			SetWindowText(hWnd, L"F Was Pressed");
		break;

	case WM_KEYUP:
		if (wParam == 'F')
			SetWindowText(hWnd, L"F Was Released");
		break;
	case WM_LBUTTONDOWN:
		POINTS pt = MAKEPOINTS(lParam);
		std::ostringstream oss;
		oss << "(" << pt.x << ", " << pt.y << ")";
		SetWindowText(hWnd, StringConverter::StringToWide(oss.str().c_str()).c_str());
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

RenderWindow::~RenderWindow()
{
}

void RenderWindow::Initialize()
{
	// Create the window class used for creating the window to be rendered.
	CreateWinClass();

	// Creates and shows the window using the class created above.
	CreateRenderWindow();
}

BOOL RenderWindow::Update()
{
	while ((gResult = GetMessage(&message, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	if (gResult == -1)
		return -1;

	return message.wParam;
}

void RenderWindow::CreateRenderWindow()
{
	//Create and show window
	HWND hWnd = CreateWindowEx(0, L"MizuClass", L"Mizu", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 200, 200, 640, 640, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, SW_SHOW);
}

void RenderWindow::CreateWinClass()
{
	// Create and setup Window Class
	WNDCLASSEX windowClass = { 0 };

	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = L"MizuClass";
	windowClass.hIconSm = nullptr;

	RegisterClassEx(&windowClass);
}