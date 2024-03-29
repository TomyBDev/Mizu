#pragma once

// Standard
#include <iostream>
#include <sstream>
#include <memory>
#include <wrl.h>
#include <fstream>

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

//xAudio2
#include <xaudio2.h>

// ImGui
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// Platform Specific
#ifdef MZ_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef MZ_DEBUG
#include <comdef.h>
#define CHECK_ERROR(...) if (FAILED(__VA_ARGS__)) { _com_error err(__VA_ARGS__); LOG_ERROR(StringConverter::WcharToString(err.ErrorMessage()));}
#else
#define CHECK_ERROR(...) __VA_ARGS__
#endif

struct DirectionalLight
{
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
};