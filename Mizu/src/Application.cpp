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
	orthoMesh = new OrthoMesh(gfx->GetDevice(), 1000, 1000, 0, 0);

	// Create Shaders
	//normalShader = new NormalShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader = new SolverShader(gfx->GetDevice(), gfx->GetDeviceContext());
	waterShader = new WaterShader(gfx->GetDevice(), gfx->GetDeviceContext());

	// Create Textures
	startingConditionTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), contentPath L"Content/StartingConditionTexture2.png");
	waterTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), contentPath L"Content/WaterTexture.png");

	// Render Textures
	newRenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), 1000, 1000, 0.1f, 200.f);
	oldRenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), 1000, 1000, 0.1f, 200.f);

	// Lighting
	light.direction = XMFLOAT3(0.5f, -0.5f, 0.f);
	light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f);
	light.diffuse = XMFLOAT4(0.6f, 0.6f, 0.8f, 1.f);

	waterScale.r[0] = { 0.1f,0,0,0 };
	waterScale.r[1] = { 0,0.1f,0,0 };
	waterScale.r[2] = { 0,0,0.1f,0 };
	waterScale.r[3] = { -50.f,-5.f,-50.f,1.0f };

	// Store initial condition in the old render texture buffer.
	SolverPass(startingConditionTexture->GetShaderResourceView(), 0.0166f);
	newRenderTexture.swap(oldRenderTexture);
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

	SolverPass(oldRenderTexture->GetShaderResourceView(), dt);

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
	//waterShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * waterScale, viewMatrix, projectionMatrix, newRenderTexture->GetShaderResourceView(), waterTexture->GetShaderResourceView(), light);
	waterShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * waterScale, viewMatrix, projectionMatrix, newRenderTexture->GetShaderResourceView(), newRenderTexture->GetShaderResourceView(), light);
	waterShader->Render(planeMesh->GetIndexCount());

	Imgui();
	graphics->EndFrame();
	
}

void Application::HandleInput(float dt)
{
	//Handles the input responses for the camera.
	camera->HandleInput(inputManager, dt);
}

void Application::SolverPass(ID3D11ShaderResourceView* srv, float dt)
{
	if (!graphics)
		return;

	newRenderTexture->SetRenderTarget(graphics->GetDeviceContext());
	// No need to clear render target, all pixels will be overwritten

	XMMATRIX worldMatrix = graphics->GetWorldMatrix();
	XMMATRIX orthoMatrix = newRenderTexture->GetOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->GetOrthoViewMatrix();

	graphics->SetZBuffer(false);

	orthoMesh->SendData(graphics->GetDeviceContext());
	solverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, srv, dt);
	solverShader->Render(orthoMesh->GetIndexCount());

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
