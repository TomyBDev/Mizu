#include "mzpch.h"

#include "System/RenderWindow.h"
#include "Input/InputManager.h"
#include "Application.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Initialise Logger
	Log::Init();

	// Create objects for window, input, and application.
	RenderWindow renderWindow(hInstance);
	InputManager inputManager;
	Application app(&inputManager);

	LOG_INFO("Started Initilaising Render Window...");
	renderWindow.Initialize(&inputManager);
	LOG_INFO("Render window initialised.");

	// Main Loop
	while (renderWindow.Update())
	{
		app.Update(0.166f);
		app.Render();
		LOG_FLUSH();
	}

	return 0;
}

