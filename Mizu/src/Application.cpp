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


	floorMesh = new PlaneMesh(graphics->GetDevice(), 10.f, 10.f);

	// Create Shaders
	normalShader = new NormalShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader = new SolverShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader2 = new SolverShader2(gfx->GetDevice(), gfx->GetDeviceContext());
	waterShader = new WaterShader(gfx->GetDevice(), gfx->GetDeviceContext());

	// Create Textures
	waterTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), contentPath L"Content/WaterTexture.png");
	floorTexture = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), contentPath L"Content/TileTexture.png");

	// Lighting
	light.direction = XMFLOAT3(0.5f, -0.5f, 0.f);
	light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f);
	light.diffuse = XMFLOAT4(0.6f, 0.6f, 0.8f, 1.f);

	resolution = resolutions.at(resolutionItem);

	XMMATRIX transMat = XMMatrixTranslation(-50.f, 0, -25.f);
	XMMATRIX scaleMat = XMMatrixScaling(11.f, 1.f, 11.f);
	floorScale = scaleMat * transMat;

	Init();

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

	graphics->ClearBuffer(0.9f, 0.6f, 0.4f);

	XMMATRIX worldMatrix = graphics->GetWorldMatrix();
	XMMATRIX viewMatrix = camera->GetViewMatrix();
	XMMATRIX projectionMatrix = graphics->GetProjectionMatrix();

	// Render Floor
	floorMesh->SendData(graphics->GetDeviceContext());
	normalShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * floorScale, viewMatrix, projectionMatrix, floorTexture->GetShaderResourceView(), 25.f);
	normalShader->Render(floorMesh->GetIndexCount());

	// Render Water
	planeMesh->SendData(graphics->GetDeviceContext());
	waterShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * waterScale, viewMatrix, projectionMatrix, pass2RenderTexture->GetShaderResourceView(), waterTexture->GetShaderResourceView(), light, camera, shallowColor, deepColor, strength);
	waterShader->Render(planeMesh->GetIndexCount());

	Imgui();
	graphics->EndFrame();
	
}

void Application::Init()
{

	// Create Mesh
	planeMesh = new PlaneMesh(graphics->GetDevice(), resolution, resolution);
	orthoMesh = new OrthoMesh(graphics->GetDevice(), resolution, resolution, 0, 0);

	// Create Textures
	std::string s = contentPathS;
	s.append("Content/StartingConditionTexture" + std::to_string(resolution) + ".png");
	startingConditionTexture = new Texture(graphics->GetDevice(), graphics->GetDeviceContext(), StringConverter::StringToWide(s));

	pass1RenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), resolution, resolution, 0.1f, 200.f);
	pass2RenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), resolution, resolution, 0.1f, 200.f);
	oldRenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), resolution, resolution, 0.1f, 200.f);

	const float scale = 100.f / static_cast<float>(resolution);
	XMMATRIX transMat = XMMatrixTranslation(-50.f, 0, -25.f);
	XMMATRIX scaleMat = XMMatrixScaling(scale, 1.f, scale);
	waterScale = scaleMat * transMat;
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
	graphics->SetAlpha(false);

	orthoMesh->SendData(graphics->GetDeviceContext());
	solverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView(), dt, resolution);
	solverShader->Render(orthoMesh->GetIndexCount());

	pass2RenderTexture->SetRenderTarget(graphics->GetDeviceContext());
	// No need to clear render target, all pixels will be overwritten

	orthoMesh->SendData(graphics->GetDeviceContext());
	solverShader2->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView(), pass1RenderTexture->GetShaderResourceView(), dt, resolution);
	solverShader2->Render(orthoMesh->GetIndexCount());

	graphics->SetZBuffer(true);
	graphics->SetAlpha(true);
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
	normalShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, srv, 1.f, true);
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

	ImGui::SliderFloat("Depth Strength", &strength, 0.1f, 100.f);

	ImGui::ColorPicker4("Shallow Color", shallowColor);
	ImGui::ColorPicker4("Deep Color", deepColor);

	const char* resolutionLabels[] = {"128x128", "256x256", "512x512", "1024x1024"};

	if (ImGui::Combo("Resolution", &resolutionItem, resolutionLabels, IM_ARRAYSIZE(resolutionLabels)))
	{
		resolution = resolutions.at(resolutionItem);
		Restart();
	}

	if (ImGui::Button("Reset"))
	{
		Restart();
	}

	/** End of ImGui Rendering. */
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Application::Restart()
{
	// Clean up
	if (planeMesh)
	{
		delete planeMesh;
		planeMesh = nullptr;
	}

	if (orthoMesh)
	{
		delete orthoMesh;
		orthoMesh = nullptr;
	}

	if (startingConditionTexture)
	{
		delete startingConditionTexture;
		startingConditionTexture = nullptr;
	}

	if (pass1RenderTexture)
		pass1RenderTexture.release();
	

	if (pass2RenderTexture)
		pass2RenderTexture.release();

	if (oldRenderTexture)
		oldRenderTexture.release();

	// Re-Initialise
	Init();

	// Store initial condition in the old render texture buffer.
	SetRenderTexturePass(pass2RenderTexture, startingConditionTexture->GetShaderResourceView());
}
