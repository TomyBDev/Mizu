#include "mzpch.h"

#include "System/RenderWindow.h"
#include "Input/InputManager.h"
#include "Application.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Initialise Logger
	Log::Init();

	RenderWindow renderWindow(hInstance);
	InputManager inputManager;
	Application app(&inputManager);

	renderWindow.Initialize(&inputManager);

	while (renderWindow.Update())
	{
		app.Update(0.166f);
		app.Render();
	}

	return 0;
}

