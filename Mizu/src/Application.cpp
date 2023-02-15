#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>

#include <Graphics/Graphics.h>
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

#include <Geometry/TriangleMesh.h>
#include <Graphics/Shaders/NormalShader.h>

Application::Application(InputManager* input, Graphics* gfx)
	: inputManager(input),
	graphics(gfx)
{
	triangleMesh = new TriangleMesh(gfx->GetDevice());
	normalShader = new NormalShader(gfx->GetDevice(), gfx->GetDeviceContext());
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
		graphics->ClearBuffer(0.4f, 0.6f, 0.9f);
		graphics->SetRenderTarget();

		triangleMesh->SendData(graphics->GetDeviceContext());
		normalShader->Render(triangleMesh->GetIndexCount());


		Imgui();
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

void Application::Imgui()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/** Initialise the ImGui Window Size. */
	ImGui::SetNextWindowSize(ImVec2(500.f, 540.f));

	/** Begin Menu */
	ImGui::Begin("Settings:");

	ImGui::Text("Hello, world!");

	/** End of ImGui Rendering. */
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
