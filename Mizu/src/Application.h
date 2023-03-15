#pragma once

#include <Graphics/RenderTexture.h>

class InputManager;

class Application
{
public:
	Application(InputManager* input, class Graphics* gfx);
	~Application();

	void Update(float dt);
	void Render();
private:
	void Init();

	void HandleInput(float dt);

	void SolverPass(float dt);

	void SetRenderTexturePass(std::unique_ptr<RenderTexture>& renderTexture, ID3D11ShaderResourceView* srv);

	void Imgui();
	void Restart();

	InputManager* inputManager;
	Graphics* graphics;
	class Camera* camera;

	// Mesh
	class PlaneMesh* planeMesh;
	class OrthoMesh* orthoMesh;

	PlaneMesh* floorMesh;

	// Shaders
	class NormalShader* normalShader;
	class SolverShader* solverShader;
	class SolverShader2* solverShader2;
	class WaterShader* waterShader;

	// Textures
	class Texture* waterTexture;
	Texture* startingConditionTexture;
	Texture* floorTexture;

	// Render Textures
	std::unique_ptr<RenderTexture> pass1RenderTexture;
	std::unique_ptr<RenderTexture> pass2RenderTexture;
	std::unique_ptr<RenderTexture> oldRenderTexture;

	// Lights
	DirectionalLight light;

	// Matrices
	DirectX::XMMATRIX waterScale;
	DirectX::XMMATRIX floorScale;

	float frameRate = 0;
	float cameraSpeed = 10.f;
	float timeElapsed = 0;
	int currentItem = 0;
	int resolution;
	int resolutionItem = 3;
	float strength = 60.f;
	float shallowColor[4] = { 0.046875f, 0.7734375f, 0.66015625f, 0.35546875f };
	float deepColor[4] = { 0.0390625f, 0.35546875f, 0.6796875f, 0.7578125f };

	const std::unordered_map<int, int> resolutions = {{0,128}, {1,256}, {2,512}, {3,1024}};
};