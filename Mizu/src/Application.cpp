#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>

#include <Graphics/Graphics.h>
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

Application::Application(InputManager* input, Graphics* gfx)
	: inputManager(input),
	graphics(gfx)
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
	if (graphics)
	{
		graphics->ClearBuffer(1, 0, 0);

		graphics->DrawTriangle();

		graphics->EndFrame();
	}
		
}

void Application::HandleInput()
{

	if (inputManager->GetMouse()->IsLeftDown())
	{
		LOG_TRACE("Good Morning");
		OutputDebugString(L"Mouse1\n");
	}

	if (inputManager->GetMouse()->IsRightDown())
	{
		OutputDebugString(L"Mouse2\n");
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