#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>

#include <Graphics/Graphics.h>
#include <Graphics/Camera.h>

#include <Geometry/PlaneMesh.h>
#include <Geometry/OrthoMesh.h>

#include <Graphics/Shaders/NormalShader.h>
#include <Graphics/Shaders/SolverShader2.h>
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
	planeMesh = new PlaneMesh(gfx->GetDevice(), 100, 100);
	orthoMesh = new OrthoMesh(gfx->GetDevice(), 100, 100, 0, 0);

	// Create Shaders
	normalShader = new NormalShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader = new SolverShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader2 = new SolverShader2(gfx->GetDevice(), gfx->GetDeviceContext());
	waterShader = new WaterShader(gfx->GetDevice(), gfx->GetDeviceContext());

	// Create Textures
	startingConditionTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), contentPath L"Content/StartingConditionTexture2.png");
	waterTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), contentPath L"Content/WaterTexture.png");

	// Render Textures
	pass1RenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), 100, 100, 0.1f, 200.f);
	pass2RenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), 100, 100, 0.1f, 200.f);
	oldRenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), 100, 100, 0.1f, 200.f);

	// Lighting
	light.direction = XMFLOAT3(0.5f, -0.5f, 0.f);
	light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f);
	light.diffuse = XMFLOAT4(0.6f, 0.6f, 0.8f, 1.f);

	waterScale.r[0] = { 1.f,0,0,0 };
	waterScale.r[1] = { 0,1.f,0,0 };
	waterScale.r[2] = { 0,0,1.f,0 };
	waterScale.r[3] = { -5.f,-5.f,-5.f,1.0f };

	// Store initial condition in the old render texture buffer.
	SetRenderTexturePass(oldRenderTexture, startingConditionTexture->GetShaderResourceView());
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

	SolverPass(dt);

	Render();

	oldRenderTexture.swap(pass2RenderTexture);
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
	waterShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * waterScale, viewMatrix, projectionMatrix, pass2RenderTexture->GetShaderResourceView(), waterTexture->GetShaderResourceView(), light, currentItem);
	waterShader->Render(planeMesh->GetIndexCount());

	Imgui();
	graphics->EndFrame();
	
}

void Application::HandleInput(float dt)
{
	//Handles the input responses for the camera.
	camera->HandleInput(inputManager, dt);
}

void Application::SolverPass(float dt)
{
	if (!graphics)
		return;

	pass1RenderTexture->SetRenderTarget(graphics->GetDeviceContext());
	// No need to clear render target, all pixels will be overwritten

	XMMATRIX worldMatrix = graphics->GetWorldMatrix();
	XMMATRIX orthoMatrix = oldRenderTexture->GetOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->GetOrthoViewMatrix();

	graphics->SetZBuffer(false);

	orthoMesh->SendData(graphics->GetDeviceContext());
	solverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView(), dt);
	solverShader->Render(orthoMesh->GetIndexCount());

	pass2RenderTexture->SetRenderTarget(graphics->GetDeviceContext());
	// No need to clear render target, all pixels will be overwritten

	orthoMesh->SendData(graphics->GetDeviceContext());
	solverShader2->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView(), pass1RenderTexture->GetShaderResourceView(), dt);
	solverShader2->Render(orthoMesh->GetIndexCount());

	graphics->SetZBuffer(true);
	graphics->SetBackBufferRenderTarget();
}

void Application::SetRenderTexturePass(std::unique_ptr<RenderTexture>& renderTexture, ID3D11ShaderResourceView* srv)
{
	if (!graphics)
		return;

	renderTexture->SetRenderTarget(graphics->GetDeviceContext());
	// No need to clear render target, all pixels will be overwritten

	XMMATRIX worldMatrix = graphics->GetWorldMatrix();
	XMMATRIX orthoMatrix = renderTexture->GetOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->GetOrthoViewMatrix();

	graphics->SetZBuffer(false);

	orthoMesh->SendData(graphics->GetDeviceContext());
	normalShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, srv);
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

	const char* labels[] = {"Texture", "Height", "uVel", "vVel"};

	ImGui::Combo("Render Mode", &currentItem, labels, IM_ARRAYSIZE(labels));

	if (ImGui::Button("Reset"))
	{
		SetRenderTexturePass(pass2RenderTexture,startingConditionTexture->GetShaderResourceView());
	}

	/** End of ImGui Rendering. */
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
