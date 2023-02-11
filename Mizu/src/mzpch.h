#pragma once

#include <iostream>
#include <sstream>
#include <memory>

// Time
#include <ctime>

#include <Tools/StringConvertor.h>
#include <Tools/Log.h>

// ImGui
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#ifdef MZ_PLATFORM_WINDOWS
#include <Windows.h>
#endif