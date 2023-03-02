#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>

#include <Graphics/Graphics.h>
#include <Graphics/Camera.h>

#include <Geometry/PlaneMesh.h>
#include <Geometry/OrthoMesh.h>

#include <Graphics/Shaders/NormalShader.h>
#include <Graphics/Shaders/SolverShader.h>
#include <Graphics/Shaders/WaterShader.h>

#include <Graphics/Texture.h>

Application::Application(InputManager* input, Graphics* gfx)
	: inputManager(input),
	graphics(gfx)
{
	LOG_INFO("Initilaising camera...");
	camera = new Camera(gfx->GetHWND());
	camera->SetSpeed(cameraSpeed);
	LOG_INFO("Camera initialised.");

	// Create Mesh
	planeMesh = new PlaneMesh(gfx->GetDevice(), 1000, 1000);
	orthoMesh = new OrthoMesh(gfx->GetDevice(), 1000, 1000, -(1280 / 2) + (1000 / 2), (-720 / 2) - (1000 / 2));

	// Create Shaders
	normalShader = new NormalShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader = new SolverShader(gfx->GetDevice(), gfx->GetDeviceContext());
	waterShader = new WaterShader(gfx->GetDevice(), gfx->GetDeviceContext());

	// Create Textures
	waterTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), L"../Content/WaterTexture.png");
	//waterTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), L"Content/WaterTexture.png");
	startingConditionTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), L"../Content/StartingConditionTexture.png");
	//startingConditionTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), L"Content/StartingConditionTexture.png");

	// Render Textures
	newRenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), 1280, 720, 0.1f, 200.f);
	oldRenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), 1280, 720, 0.1f, 200.f);

	waterScale.r[0] = { 0.1f,0,0,0 };
	waterScale.r[1] = { 0,0.1f,0,0 };
	waterScale.r[2] = { 0,0,0.1f,0 };
	waterScale.r[3] = { -50.f,-5.f,-50.f,1.0f };
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

	SolverPass();

	Render();

	newRenderTexture.swap(oldRenderTexture);
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
	waterShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * waterScale, viewMatrix, projectionMatrix, newRenderTexture->GetShaderResourceView(), waterTexture->GetShaderResourceView());
	waterShader->Render(planeMesh->GetIndexCount());

	Imgui();
	graphics->EndFrame();
	
}

void Application::HandleInput(float dt)
{
	//Handles the input responses for the camera.
	camera->HandleInput(inputManager, dt);
}

/*void Application::SolverPass()
{
	if (!graphics)
		return;

	newRenderTexture->SetRenderTarget(graphics->GetDeviceContext());
	newRenderTexture->ClearRenderTarget(graphics->GetDeviceContext(), 0.f, 0.f, 0.f);

	XMMATRIX worldMatrix = graphics->GetWorldMatrix();
	XMMATRIX orthoMatrix = graphics->GetOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->GetOrthoViewMatrix();

	graphics->SetZBuffer(false);

	orthoMesh->SendData(graphics->GetDeviceContext());
	if (bFirstFrame)
	{
		bFirstFrame = false;
		solverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, startingConditionTexture->GetShaderResourceView());
	}
	else
	{
		solverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView());
	}
	solverShader->Render(orthoMesh->GetIndexCount());


	graphics->SetZBuffer(true);
	graphics->SetBackBufferRenderTarget();
}*/

void Application::SolverPass()
{
	if (!graphics)
		return;

	newRenderTexture->SetRenderTarget(graphics->GetDeviceContext());
	newRenderTexture->ClearRenderTarget(graphics->GetDeviceContext().Get(), 0.f, 0.f, 0.f);

	XMMATRIX worldMatrix = graphics->GetWorldMatrix();
	XMMATRIX orthoMatrix = graphics->GetOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->GetOrthoViewMatrix();

	graphics->SetZBuffer(false);

	orthoMesh->SendData(graphics->GetDeviceContext());
	normalShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, startingConditionTexture->GetShaderResourceView(), 0.f);
	normalShader->Render(orthoMesh->GetIndexCount());


	graphics->SetZBuffer(true);
	graphics->SetBackBufferRenderTarget();
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
