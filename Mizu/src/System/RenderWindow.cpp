#include <mzpch.h>
#include <System/RenderWindow.h>

#include <Input/InputManager.h>

#include "Graphics/Graphics.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	RenderWindow* pThis = nullptr;

	if (msg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (RenderWindow*)pCreate->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
	}

	if (msg == WM_CLOSE)
		PostQuitMessage(0);

	pThis = (RenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (pThis)
		pThis->WindowProc(hWnd, msg, wParam, lParam);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

RenderWindow::~RenderWindow()
{
}

void RenderWindow::Initialize(InputManager* input)
{
	inputManager = input;

	// Create the window class used for creating the window to be rendered.
	CreateWinClass();

	// Creates and shows the window using the class created above.
	CreateRenderWindow();
}

void RenderWindow::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//Keyboard
	case WM_KEYDOWN:
		inputManager->GetKeyboard()->OnPressed(wParam);
		break;

	case WM_KEYUP:
		inputManager->GetKeyboard()->OnReleased(wParam);
		break;

		//Mouse
	case WM_LBUTTONDOWN:
		inputManager->GetMouse()->OnLeftPressed();
		break;
	case WM_LBUTTONUP:
		inputManager->GetMouse()->OnLeftReleased();
		break;

	case WM_RBUTTONDOWN:
		inputManager->GetMouse()->OnRightPressed();
		break;
	case WM_RBUTTONUP:
		inputManager->GetMouse()->OnRightReleased();
		break;

	case WM_MBUTTONDOWN:
		inputManager->GetMouse()->OnMiddlePressed();
		break;
	case WM_MBUTTONUP:
		inputManager->GetMouse()->OnMiddleReleased();
		break;

	case WM_MOUSEMOVE:
		POINTS pt = MAKEPOINTS(lParam);
		inputManager->GetMouse()->OnMouseMove(pt.x, pt.y);
		break;
	}

}

bool RenderWindow::Update()
{
	if(PeekMessage(&message, nullptr, 0, 0, 1) >0)
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	if (message.message == WM_QUIT)
		return false;

	return true;
}

void RenderWindow::CreateRenderWindow()
{
	LOG_INFO("Started creating window class...");

	//Create window using window class.
	HWND hWnd = CreateWindowEx(0, windowName.c_str(), windowName.c_str(), WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 200, 200, width, height, nullptr, nullptr, hInstance, this);

	// If Window Creation Failed, exit and log error.
	if (hWnd == NULL)
	{
		LOG_CRITICAL(StringConverter::GetLastErrorAsString());
		exit(-1);
	}

	// Show the created window.
	ShowWindow(hWnd, SW_SHOW);

	LOG_INFO("Window class created.");

	LOG_INFO("Starting creation of graphics object...");
	graphics = new Graphics(hWnd);
	LOG_INFO("Graphics object created.");

	LOG_INFO("Starting initialisation of ImGui...");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(graphics->GetDevice().Get(), graphics->GetDeviceContext().Get());
	ImGui::StyleColorsDark();
	LOG_INFO("ImGui initialised.");
}

void RenderWindow::CreateWinClass()
{
	LOG_INFO("Started registering window class...");

	// Create and setup Window Class
	WNDCLASSEX windowClass = { 0 };

	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_OWNDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_MIZUICON),IMAGE_ICON,32,28, LR_LOADTRANSPARENT));
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = nullptr;
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = windowName.c_str();
	windowClass.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_MIZUICON), IMAGE_ICON, 32, 28, LR_LOADTRANSPARENT));

	if (!RegisterClassEx(&windowClass))
	{
		LOG_CRITICAL(StringConverter::GetLastErrorAsString());
		exit(-1);
	}

	LOG_INFO("Window class registered.");
}