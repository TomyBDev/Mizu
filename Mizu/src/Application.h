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

	class Wavefront* model;

	// Shaders
	class TextureShader* textureShader;
	class SolverShader* solverShader;
	class SolverShader2* solverShader2;
	class WaterShader* waterShader;
	class WavefrontShader* wavefrontShader;

	// Textures
	class Texture* startingConditionTexture;
	class TextureCube* skyTextureCube;

	// Render Textures
	std::unique_ptr<RenderTexture> pass1RenderTexture;
	std::unique_ptr<RenderTexture> pass2RenderTexture;
	std::unique_ptr<RenderTexture> oldRenderTexture;

	// Lights
	DirectionalLight light;

	// Matrices
	DirectX::XMMATRIX waterScale;

	// Frame rate to be displayed
	float frameRate = 0;

	// Camera Control
	float cameraSpeed = 10.f;

	// Water Resolution Control
	int resolution;
	int resolutionItem = 3;
	const std::unordered_map<int, int> resolutions = { {0,128}, {1,256}, {2,512}, {3,1024} };

	// Water Shader Control
	float strength = 55.f;
	float shallowColor[4] = { 0.046875f, 0.7734375f, 0.66015625f, 0.35546875f };
	float deepColor[4] = { 0.0390625f, 0.35546875f, 0.6796875f, 0.7578125f };
};