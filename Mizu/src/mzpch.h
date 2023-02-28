#pragma once

// Standard
#include <iostream>
#include <sstream>
#include <memory>
#include <wrl.h>

// Resources
#include <Resources/resource.h>

// Time
#include <ctime>

// Tools
#include <Tools/StringConvertor.h>
#include <Tools/Log.h>
#include <spdlog/spdlog.h>
#include <DirectXMath.h>
#include <gdiplus.h>

// D3D11
#include <d3d11.h>
#include <d3dcompiler.h>

// ImGui
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// Platform Specific
#ifdef MZ_PLATFORM_WINDOWS
#include <Windows.h>
#endif