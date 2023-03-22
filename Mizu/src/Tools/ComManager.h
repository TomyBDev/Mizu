#pragma once
#include <mzpch.h>

class ComManager
{
public:
	ComManager()
	{
		// Ensure COM is initialized.
		CHECK_ERROR(CoInitializeEx(NULL, COINIT_MULTITHREADED));
	}

	~ComManager()
	{
		// Uninitialize COM
		CoUninitialize();
	}
};