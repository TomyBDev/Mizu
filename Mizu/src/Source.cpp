#include "mzpch.h"

#include "System/RenderWindow.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	RenderWindow renderWindow(hInstance);

	renderWindow.Initialize();

	const BOOL exitStatus = renderWindow.Update();
	return exitStatus;
}

