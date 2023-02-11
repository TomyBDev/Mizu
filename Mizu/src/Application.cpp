#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>
#include <spdlog/logger.h>

#include "Input/Keyboard.h"
#include "Input/Mouse.h"

Application::Application(InputManager* input)
	: inputManager(input)
{
}

Application::~Application()
{
}

void Application::Update(float dt)
{
	HandleInput();

}

void Application::Render()
{
}

void Application::HandleInput()
{

	if (inputManager->GetMouse()->IsLeftDown())
	{
		Log::GetLogger()->trace("Left Mouse Is Down");
		//OutputDebugString(L"Left Mouse Is Down\n");
	}

	if (inputManager->GetMouse()->IsRightDown())
	{
		OutputDebugString(L"Right Mouse Is Down\n");
	}

	if (inputManager->GetMouse()->IsMiddleDown())
	{
		OutputDebugString(L"Middle Mouse Is Down\n");
	}

	if (inputManager->GetKeyboard()->IsKeyDown(0x57))
	{
		OutputDebugString(L"W\n");
	}

	if (inputManager->GetKeyboard()->IsKeyDown(0x41))
	{
		OutputDebugString(L"A\n");
	}

	if (inputManager->GetKeyboard()->IsKeyDown(0x53))
	{
		OutputDebugString(L"S\n");
	}

	if (inputManager->GetKeyboard()->IsKeyDown(0x44))
	{
		OutputDebugString(L"D\n");
	}
}