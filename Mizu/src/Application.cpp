#include <mzpch.h>
#include <Application.h>

#include <Input/InputManager.h>
#include <Audio/SoundSystem.h>

#include <Graphics/Graphics.h>
#include <Graphics/Camera.h>

#include <Geometry/PlaneMesh.h>
#include <Geometry/OrthoMesh.h>
#include <Geometry/MaterialObject.h>
#include <Geometry/TextureObject.h>
#include <Geometry/CubeMesh.h>

#include <Graphics/Shaders/TextureShader.h>
#include <Graphics/Shaders/SolverShader2.h>
#include <Graphics/Shaders/SolverShader.h>
#include <Graphics/Shaders/WaterShader.h>
#include <Graphics/Shaders/MaterialObjectShader.h>
#include <Graphics/Shaders/TextureObjectShader.h>
#include <Graphics/Shaders/SkyShader.h>

#include <Graphics/Texture.h>
#include <Graphics/TextureCube.h>

Application::Application(InputManager* input, Graphics* gfx)
	: inputManager(input),
	graphics(gfx)
{
	LOG_INFO("Initilaising camera...");
	camera = new Camera(gfx->GetHWND());
	camera->SetSpeed(cameraSpeed);
	LOG_INFO("Camera initialised.");

	ambient = new Sound("Ambient.wav");
	ambient->Play(true);

	model = new MaterialObject(graphics->GetDevice(), "Pool/Pool.obj");
	palmTree = new TextureObject(graphics->GetDevice(), "PalmTree/palm_tree.obj");
	cubeMesh = new CubeMesh(graphics->GetDevice());

	// Create Shaders
	textureShader = new TextureShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader = new SolverShader(gfx->GetDevice(), gfx->GetDeviceContext());
	solverShader2 = new SolverShader2(gfx->GetDevice(), gfx->GetDeviceContext());
	waterShader = new WaterShader(gfx->GetDevice(), gfx->GetDeviceContext());
	materialObjectShader = new MaterialObjectShader(gfx->GetDevice(), gfx->GetDeviceContext());
	textureObjectShader = new TextureObjectShader(gfx->GetDevice(), gfx->GetDeviceContext());
	skyShader = new SkyShader(gfx->GetDevice(), gfx->GetDeviceContext());

	skyTextureCube = new TextureCube(gfx->GetDevice(), gfx->GetDeviceContext(), L"Sky/SkyCubeMap.png");
	palmTreeDiffuse = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), L"PalmTree/diffuse.png");
	palmTreeNormal = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), L"PalmTree/normal.png");
	palmTreeSpecular = new Texture(gfx->GetDevice(), gfx->GetDeviceContext(), L"PalmTree/specular.png");

	// Lighting
	light.direction = XMFLOAT3(-1.f, -0.5f, 1.f);
	light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f);
	light.diffuse = XMFLOAT4(0.6f, 0.6f, 0.8f, 1.f);

	resolution = resolutions.at(resolutionItem);

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

	// Sky Box
	graphics->SetZBuffer(false);
	cubeMesh->SendData(graphics->GetDeviceContext());
	skyShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * XMMatrixScaling(10.f, 10.f, 10.f) * XMMatrixRotationX(-1.570795f) * XMMatrixTranslation(camera->GetPosition().m128_f32[0], camera->GetPosition().m128_f32[1], camera->GetPosition().m128_f32[2]), viewMatrix, projectionMatrix, skyTextureCube->GetShaderResourceView());
	skyShader->Render(cubeMesh->GetIndexCount());
	graphics->SetZBuffer(true);

	// Model Floor
	model->SendData(graphics->GetDeviceContext());
	materialObjectShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * XMMatrixScaling(3.15f, 3.15f, 3.15f) * XMMatrixTranslation(0.f,6.2f, 25.f), viewMatrix, projectionMatrix, light);
	materialObjectShader->Render(model->GetIndexCount());

	// Render Water
	planeMesh->SendData(graphics->GetDeviceContext());
	waterShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * waterScale, viewMatrix, projectionMatrix, pass2RenderTexture->GetShaderResourceView(), skyTextureCube->GetShaderResourceView(), light, camera, shallowColor, deepColor, strength);
	waterShader->Render(planeMesh->GetIndexCount());

	// Palm Tree
	graphics->SetBothSides(true);
	palmTree->SendData(graphics->GetDeviceContext());
	textureObjectShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * XMMatrixScaling(0.05f, 0.05f, 0.05f), viewMatrix, projectionMatrix, light, palmTreeDiffuse->GetShaderResourceView(), palmTreeNormal->GetShaderResourceView(), palmTreeSpecular->GetShaderResourceView());
	textureObjectShader->Render(palmTree->GetIndexCount());
	graphics->SetBothSides(false);

	Imgui();
	graphics->EndFrame();
	
}

void Application::Init()
{
	// Create Mesh
	planeMesh = new PlaneMesh(graphics->GetDevice(), resolution, resolution);
	orthoMesh = new OrthoMesh(graphics->GetDevice(), resolution, resolution, 0, 0);

	// Create Textures
	std::string s = "StartingCondition/SC";
	s.append(std::to_string(resolution) + ".png");
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
	textureShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, srv);
	textureShader->Render(orthoMesh->GetIndexCount());

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

	if (ImGui::CollapsingHeader("Camera Settings"))
	{

		ImGui::Text("Camera Pos: x: %.1f, y: %.1f, z: %.1f", camera->GetPosition().m128_f32[0], camera->GetPosition().m128_f32[1], camera->GetPosition().m128_f32[2]);
		ImGui::Text("Camera Rot: x: %.1f, y: %.1f, z: %.1f", camera->GetRotation().m128_f32[0], camera->GetRotation().m128_f32[1], camera->GetRotation().m128_f32[2]);

		if (ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1f, 1000.f))
		{
			camera->SetSpeed(cameraSpeed);
		}
	}
	if (ImGui::CollapsingHeader("Water Shader Settings"))
	{
		ImGui::SliderFloat("Depth Strength", &strength, 0.1f, 100.f);
		if (ImGui::TreeNode("Shallow Water Color"))
		{
			ImGui::ColorPicker4("Shallow Color", shallowColor);
			ImGui::TreePop();
			ImGui::Separator();
		}
		if (ImGui::TreeNode("Deep Water Color"))
		{
			ImGui::ColorPicker4("Deep Color", deepColor);
			ImGui::TreePop();
			ImGui::Separator();
		}
	}
	
	if (ImGui::CollapsingHeader("Simulation Control"))
	{
		const char* resolutionLabels[] = { "128x128", "256x256", "512x512", "1024x1024" };

		if (ImGui::Combo("Resolution", &resolutionItem, resolutionLabels, IM_ARRAYSIZE(resolutionLabels)))
		{
			resolution = resolutions.at(resolutionItem);
			Restart();
		}

		if (ImGui::Button("Reset"))
		{
			Restart();
		}

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
