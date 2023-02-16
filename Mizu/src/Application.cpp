#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>

#include <Graphics/Graphics.h>
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include <Graphics/Camera.h>

#include <Geometry/TriangleMesh.h>
#include <Graphics/Shaders/NormalShader.h>

Application::Application(InputManager* input, Graphics* gfx)
	: inputManager(input),
	graphics(gfx)
{
	LOG_INFO("Initilaising camera...");
	camera = new Camera(gfx->GetHWND());
	camera->SetSpeed(cameraSpeed);
	LOG_INFO("Camera initialised.");

	triangleMesh = new TriangleMesh(gfx->GetDevice());
	normalShader = new NormalShader(gfx->GetDevice(), gfx->GetDeviceContext());
}

Application::~Application()
{
}

void Application::Update(float dt)
{
	frameRate = 1.f / dt;

	HandleInput(dt);

	camera->Update();
}

void Application::Render()
{
	if (graphics)
	{
		graphics->ClearBuffer(0.4f, 0.6f, 0.9f);
		graphics->SetRenderTarget();

		XMMATRIX worldMatrix = graphics->GetWorldMatrix();
		XMMATRIX viewMatrix = camera->GetViewMatrix();
		XMMATRIX projectionMatrix = graphics->GetProjectionMatrix();

		triangleMesh->SendData(graphics->GetDeviceContext());
		normalShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
		normalShader->Render(triangleMesh->GetIndexCount());


		Imgui();
		graphics->EndFrame();
	}
		
}

void Application::HandleInput(float dt)
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

	camera->HandleInput(inputManager, dt);
}

void Application::Imgui()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/** Initialise the ImGui Window Size. */
	ImGui::SetNextWindowSize(ImVec2(300.f, 340.f));

	/** Begin Menu */
	ImGui::Begin("Settings:");

	ImGui::Text("Framerate: %.1f", frameRate);

	ImGui::Text("Camera Pos: x: %.1f, y: %.1f, z: %.1f", camera->GetPosition().m128_f32[0], camera->GetPosition().m128_f32[1], camera->GetPosition().m128_f32[2]);
	ImGui::Text("Camera Rot: x: %.1f, y: %.1f, z: %.1f", camera->GetRotation().m128_f32[0], camera->GetRotation().m128_f32[1], camera->GetRotation().m128_f32[2]);

	if (ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1f, 1000.f))
	{
		camera->SetSpeed(cameraSpeed);
	}

	/** End of ImGui Rendering. */
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
