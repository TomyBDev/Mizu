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

#include "Graphics/TextureArray.h"

Application::Application(InputManager* input, Graphics* gfx)
	: inputManager(input),
	graphics(gfx)
{
	LOG_INFO("Initilaising camera..."); LOG_FLUSH();
	camera = new Camera(gfx->GetHWND());
	camera->SetSpeed(cameraSpeed);
	LOG_INFO("Camera initialised."); LOG_FLUSH();

	LOG_INFO("Initilaising sound..."); LOG_FLUSH();
	ambient = new Sound("Ambient.wav");
	ambient->Play(true);
	LOG_INFO("Initilaising sound..."); LOG_FLUSH();

	// Skybox mesh
	cubeMesh = new CubeMesh(graphics->GetDevice());

	// Onsen Mesh
	onsen = new MaterialObject(graphics, "Onsen/onsen.obj", &onsenDiff, &onsenBump);

	// Create Shaders
	textureShader = new TextureShader(gfx->GetDevice(), gfx->GetDeviceContext());

	LW1SolverShader = new SolverShader(gfx->GetDevice(), gfx->GetDeviceContext(), L"Shaders/LW1SolverShader_vs.cso", L"Shaders/LW1SolverShader_ps.cso");
	LW2SolverShader = new SolverShader2(gfx->GetDevice(), gfx->GetDeviceContext(), L"Shaders/LW2SolverShader_vs.cso", L"Shaders/LW2SolverShader_ps.cso");

	MC1SolverShader = new SolverShader(gfx->GetDevice(), gfx->GetDeviceContext(), L"Shaders/MC1SolverShader_vs.cso", L"Shaders/MC1SolverShader_ps.cso");
	MC2SolverShader = new SolverShader2(gfx->GetDevice(), gfx->GetDeviceContext(), L"Shaders/MC2SolverShader_vs.cso", L"Shaders/MC2SolverShader_ps.cso");
	
	waterShader = new WaterShader(gfx->GetDevice(), gfx->GetDeviceContext());
	materialObjectShader = new MaterialObjectShader(gfx->GetDevice(), gfx->GetDeviceContext());
	skyShader = new SkyShader(gfx->GetDevice(), gfx->GetDeviceContext());

	// Create texture cube mesh
	skyTextureCube = new TextureCube(gfx->GetDevice(), gfx->GetDeviceContext(), L"Sky/SkyCubeMap.png");

	// Lighting
	light.direction = XMFLOAT3(-1.f, -0.5f, 1.f);
	light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f);
	light.diffuse = XMFLOAT4(0.375f, 0.375f, 0.5f, 1.f);

	// Resolution specific initialisation.
	Init();

	// Store initial condition in the old render texture buffer.
	SetRenderTexturePass(oldRenderTexture, startingConditionTexture->GetShaderResourceView());
}

Application::~Application()
{

	if (ambient)
	{
		delete ambient;
		ambient = nullptr;
	}

	if (ambient)
	{
		delete ambient;
		ambient = nullptr;
	}

	if (ambient)
	{
		delete ambient;
		ambient = nullptr;
	}

	if (ambient)
	{
		delete ambient;
		ambient = nullptr;
	}

}

void Application::Update(float dt)
{
	// Calculate framerate
	frameRate = 1.f / dt;

	// Handle user input
	HandleInput(dt);

	// Update the camera
	camera->Update();

	if (timeElapsed > 0.01667f) // Only update the simulation sixty times a second.
	{
		switch (currentSolver) // Solve using the selected solver.
		{
		case LaxWendroff:
			LWSolverPass(dt);
			break;

		case MacCormack:
			MCSolverPass(dt);
			break;

		default:
			LOG_ERROR("No valid solver selected!");
			break;
		}

		// Time elapsed since last simulation update to zero!
		timeElapsed = 0;

		// Swap the new calculate water condition into the old render texture (as pass2 will be overwritten)
		oldRenderTexture.swap(pass2RenderTexture);
	}

	// Render scene + water
	Render();

	// Increment time elapsed since last simulation update.
	timeElapsed += dt;
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
	skyShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * XMMatrixScaling(10.f, 10.f, 10.f) * XMMatrixTranslation(camera->GetPosition().m128_f32[0], camera->GetPosition().m128_f32[1], camera->GetPosition().m128_f32[2]), viewMatrix, projectionMatrix, skyTextureCube->GetShaderResourceView());
	skyShader->Render(cubeMesh->GetIndexCount());
	graphics->SetZBuffer(true);

	// Model Onsen
	graphics->SetBothSides(true);
	onsen->SendData(graphics->GetDeviceContext());
	materialObjectShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * XMMatrixScaling(2.f, 2.f, 2.f) * XMMatrixTranslation(0.f, 0.f, 0.f), viewMatrix, projectionMatrix, light, onsenDiff->GetShaderResourceView(), onsenBump->GetShaderResourceView(), camera->GetPosition());
	materialObjectShader->Render(onsen->GetIndexCount());
	graphics->SetBothSides(false);

	// Render Water
	for (int i = 0; i < numToRender; i++)
	{
		planeMesh->SendData(graphics->GetDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		waterShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix * waterMats[i], viewMatrix, projectionMatrix, oldRenderTexture->GetShaderResourceView(), skyTextureCube->GetShaderResourceView(), light, camera, shallowColor, deepColor, strength, resolution, currentTessellation, waterReflections);
		waterShader->Render(planeMesh->GetIndexCount());
	}

	// Render GUI
	Imgui();

	graphics->EndFrame();
}

void Application::Init()
{
	// Create Mesh
	planeMesh = new PlaneMesh(graphics->GetDevice(), resolution.first, resolution.second);
	orthoMesh = new OrthoMesh(graphics->GetDevice(), resolution.first, resolution.second, 0, 0);

	// Create Textures
	std::string s = "StartingCondition/SC";
	s.append(std::to_string(currentStartingCon) + ".png");
	startingConditionTexture = new Texture(graphics->GetDevice(), graphics->GetDeviceContext(), StringConverter::StringToWide(s));

	// Create render textures
	pass1RenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), resolution.first, resolution.second, 0.1f, 200.f);
	pass2RenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), resolution.first, resolution.second, 0.1f, 200.f);
	oldRenderTexture = std::make_unique<RenderTexture>(graphics->GetDevice(), resolution.first, resolution.second, 0.1f, 200.f);

	// Calculate and store matrices used for offsetting and scaling water meshes.
	const float scale = 60.f / static_cast<float>(resolution.first);
	XMMATRIX scaleMat = XMMatrixScaling(scale, 1.f, scale);
	waterMats.push_back(scaleMat * XMMatrixTranslation(0, -15.f, 9.f));
	waterMats.push_back(scaleMat * XMMatrixTranslation(-58.f, -15.f, 9.f));
	waterMats.push_back(scaleMat * XMMatrixTranslation(0, -15.f, -49.f));
	waterMats.push_back(scaleMat * XMMatrixTranslation(-58.f, -15.f, -49.f));
}

void Application::HandleInput(float dt)
{
	//Handles the input responses for the camera.
	camera->HandleInput(inputManager, dt);
}

void Application::LWSolverPass(float dt)
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
	LW1SolverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView(), dt, resolution);
	LW1SolverShader->Render(orthoMesh->GetIndexCount());

	pass2RenderTexture->SetRenderTarget(graphics->GetDeviceContext());
	// No need to clear render target, all pixels will be overwritten

	orthoMesh->SendData(graphics->GetDeviceContext());
	LW2SolverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView(), pass1RenderTexture->GetShaderResourceView(), dt, resolution);
	LW2SolverShader->Render(orthoMesh->GetIndexCount());

	graphics->SetZBuffer(true);
	graphics->SetAlpha(true);
	graphics->SetBackBufferRenderTarget();
}

void Application::MCSolverPass(float dt)
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
	MC1SolverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView(), dt, resolution);
	MC1SolverShader->Render(orthoMesh->GetIndexCount());

	pass2RenderTexture->SetRenderTarget(graphics->GetDeviceContext());
	// No need to clear render target, all pixels will be overwritten

	orthoMesh->SendData(graphics->GetDeviceContext());
	MC2SolverShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, oldRenderTexture->GetShaderResourceView(), pass1RenderTexture->GetShaderResourceView(), dt, resolution);
	MC2SolverShader->Render(orthoMesh->GetIndexCount());

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
	graphics->SetAlpha(false);

	orthoMesh->SendData(graphics->GetDeviceContext());
	textureShader->SetShaderParameters(graphics->GetDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, srv);
	textureShader->Render(orthoMesh->GetIndexCount());

	graphics->SetZBuffer(true);
	graphics->SetAlpha(true);
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

		// Slider to control camera speed.
		if (ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1f, 150.f))
		{
			camera->SetSpeed(cameraSpeed);
		}
	}

	if (ImGui::CollapsingHeader("Water Shader Settings"))
	{
		// The depth strength determines the height at which the colour blends between shallow and deep.
		ImGui::SliderFloat("Depth Strength", &strength, 0.1f, 200.f);

		// Colour picker for shallow water colour.
		if (ImGui::TreeNode("Shallow Water Color"))
		{
			ImGui::ColorPicker4("Shallow Color", shallowColor);
			ImGui::TreePop();
			ImGui::Separator();
		}

		// Colour picker for deep water colour.
		if (ImGui::TreeNode("Deep Water Color"))
		{
			ImGui::ColorPicker4("Deep Color", deepColor);
			ImGui::TreePop();
			ImGui::Separator();
		}

		// Checkbox for toggling first order reflections on water.
		ImGui::Checkbox("Reflections", &waterReflections);

		// If checkbox is ticked only render one water mesh, otherwise render all.
		if (ImGui::Checkbox("Only Render One Water", &onlyRenderOne))
		{
			if (onlyRenderOne)
				numToRender = 1;
			else
				numToRender = waterMats.size();
		}

		// Controls tessellation amount;
		const char* tesselationLabels[] = { "None", "Low", "Medium", "High"};
		ImGui::Combo("tessellation", &currentTessellation, tesselationLabels, IM_ARRAYSIZE(tesselationLabels));
	}
	
	if (ImGui::CollapsingHeader("Simulation Control"))
	{
		// Combobox to switch between solvers.
		const char* solverLabels[] = { "LaxWendroff", "MacCormack" };
		ImGui::Combo("Solver", &currentSolver, solverLabels, IM_ARRAYSIZE(solverLabels));

		// Combo box to switch starting condition
		const char* startConLabels[] = { "Condition 1", "Condition 2", "Condition 3" };
		if (ImGui::Combo("Starting Condition", &currentStartingCon, startConLabels, IM_ARRAYSIZE(startConLabels)))
		{
			// Load new starting condition texture
			delete startingConditionTexture;
			std::string s = "StartingCondition/SC";
			s.append(std::to_string(currentStartingCon) + ".png");
			startingConditionTexture = new Texture(graphics->GetDevice(), graphics->GetDeviceContext(), StringConverter::StringToWide(s));
			// Store new initial condition in the old render texture buffer.
			SetRenderTexturePass(oldRenderTexture, startingConditionTexture->GetShaderResourceView());
		}

		// Resets the current state of the simulation back to the starting conditions state.
		if (ImGui::Button("Reset"))
		{
			SetRenderTexturePass(oldRenderTexture, startingConditionTexture->GetShaderResourceView());
		}

	}
	/** End of ImGui Rendering. */
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}