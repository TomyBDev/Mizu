#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>

#include <Graphics/Graphics.h>
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include <Graphics/Camera.h>

#include <Geometry/PlaneMesh.h>
#include <Geometry/OrthoMesh.h>

#include <Graphics/Shaders/NormalShader.h>
#include <Graphics/Shaders/SolverShader.h>

Application::Application(InputManager* input, Graphics* gfx)
	: inputManager(input),
	graphics(gfx)
{
	LOG_INFO("Initilaising camera...");
	camera = new Camera(gfx->GetHWND());
	camera->SetSpeed(cameraSpeed);
	LOG_INFO("Camera initialised.");

	planeMesh = new PlaneMesh(gfx->GetDevice(), 100, 100);
	orthoMesh = new OrthoMesh(gfx->GetDevice(), 100, 100, -(1280 / 2) + (100 / 2), (-720 / 2) - (100 / 2));

	normalShader = new NormalShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader = new SolverShader(gfx->GetDevice(), gfx->GetDeviceContext());
}

Application::~Application()
{
}

void Application::Update(float dt)
{
	frameRate = 1.f / dt;
	timeElapsed += dt;

	HandleInput(dt);

	camera->Update();

	//SolverPass();

	Render();
}

void Application::Render()
{
	if (!graphics)
		return;

	graphics->ClearBuffer(0.4f, 0.6f, 0.9f);

	XMMATRIX worldMatrix = graphics->GetWorldMatrix();
	XMMATRIX viewMatrix = camera->GetViewMatrix();
	XMMATRIX projectionMatrix = graphics->GetProjectionMatrix();

	planeMesh->SendData(graphics->GetDeviceContext());
	normalShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, timeElapsed);
	normalShader->Render(planeMesh->GetIndexCount());

	Imgui();
	graphics->EndFrame();
	
}

void Application::HandleInput(float dt)
{
	//Handles the input responses for the camera.
	camera->HandleInput(inputManager, dt);
}

void Application::SolverPass()
{
	/*if (!graphics)
		return;

	newRenderTexture->setRenderTarget(graphics->GetDeviceContext());
	newRenderTexture->clearRenderTarget(graphics->GetDeviceContext(), 0.f, 0.f, 0.f, 1.0f);

	XMMATRIX worldMatrix = graphics->GetWorldMatrix();
	XMMATRIX orthoMatrix = graphics->GetOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->GetOrthoViewMatrix();

	graphics->SetZBuffer(false);

	orthoMesh->SendData(graphics->GetDeviceContext());
	if (bFirstFrame)
		solverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, textureMgr->getTexture(L"StartingCondition"), bFirstFrame);
	else
		solverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->getShaderResourceView(), bFirstFrame);
	solverShader->Render(orthoMesh->GetIndexCount());


	graphics->SetZBuffer(false);
	graphics->SetBackBufferRenderTarget();*/
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
