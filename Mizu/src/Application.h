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
	void HandleInput(float dt);

	void SolverPass(ID3D11ShaderResourceView* srv, float dt);

	void Imgui();

	InputManager* inputManager;
	Graphics* graphics;
	class Camera* camera;

	// Mesh
	class PlaneMesh* planeMesh;
	class OrthoMesh* orthoMesh;

	// Shaders
	class NormalShader* normalShader;
	class SolverShader* solverShader;
	class WaterShader* waterShader;

	// Textures
	class Texture* waterTexture;
	class Texture* startingConditionTexture;

	// Render Textures
	std::unique_ptr<RenderTexture> newRenderTexture;
	std::unique_ptr<RenderTexture> oldRenderTexture;

	// Lights
	DirectionalLight light;

	// Matrices
	DirectX::XMMATRIX waterScale;

	float frameRate = 0;
	float cameraSpeed = 10.f;
	float timeElapsed = 0;

	bool bFirstFrame = true;
};