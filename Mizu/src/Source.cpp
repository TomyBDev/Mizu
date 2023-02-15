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

	LOG_INFO("Started Initilaising Render Window...");
	renderWindow.Initialize(&inputManager);
	LOG_INFO("Render window initialised.");

	Application app(&inputManager, renderWindow.GetGraphics());

	// calculate the time between updates
	
	UINT64 clock, clock_frequency, clock_last_frame_;
	QueryPerformanceCounter((LARGE_INTEGER*)&clock_last_frame_);



	// Main Loop
	while (renderWindow.Update())
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&clock);
		QueryPerformanceFrequency((LARGE_INTEGER*)&clock_frequency);
		UINT32 cycles = (UINT32)(clock - clock_last_frame_);
		clock_last_frame_ = clock;

		app.Update((float)cycles / (float)clock_frequency);
		app.Render();
		LOG_FLUSH();
	}

	return 0;
}

