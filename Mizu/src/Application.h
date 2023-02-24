#pragma once

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

	void SolverPass();

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

	float frameRate = 0;
	float cameraSpeed = 10.f;
	float timeElapsed = 0;

	bool bFirstFrame = true;
};