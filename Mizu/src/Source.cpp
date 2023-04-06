#include "mzpch.h"

#include "Graphics/GDIPlusManager.h"
#include "System/RenderWindow.h"
#include "Input/InputManager.h"
#include "Application.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Initialise Logger
	Log::Init();

	LOG_INFO("Initalising GDIPlus...");
	static GDIPlusManager gdiPlusManager;
	LOG_INFO("GDIPlus initialised.");

	// Create objects for window, input, and application.
	RenderWindow renderWindow(hInstance);
	InputManager inputManager;

	LOG_INFO("Started Initilaising Render Window...");
	renderWindow.Initialize(&inputManager);
	LOG_INFO("Render window initialised.");

	LOG_FLUSH();

	LOG_INFO("Initilaising Application...");
	Application app(&inputManager, renderWindow.GetGraphics());
	LOG_INFO("Application initialised.");

	LOG_FLUSH();

	// calculate the time between updates
	UINT64 clock, clock_frequency, clock_last_frame_;
	QueryPerformanceCounter((LARGE_INTEGER*)&clock_last_frame_);

	LOG_INFO("Program has loaded sucessfully! Now running application.");
	LOG_FLUSH();
	// Main Loop
	while (renderWindow.Update())
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&clock);
		QueryPerformanceFrequency((LARGE_INTEGER*)&clock_frequency);
		UINT32 cycles = (UINT32)(clock - clock_last_frame_);
		clock_last_frame_ = clock;

		app.Update((float)cycles / (float)clock_frequency);
		LOG_FLUSH();
	}
	return 0;
}

