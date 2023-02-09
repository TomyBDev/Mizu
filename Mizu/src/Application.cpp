#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>

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
		OutputDebugString(L"Left Mouse Is Down\n");
	}

	if (inputManager->GetMouse()->IsRightDown())
	{
		OutputDebugString(L"Right Mouse Is Down\n");
	}

	if (inputManager->GetMouse()->IsMiddleDown())
	{
		OutputDebugString(L"Middle Mouse Is Down\n");
	}
}