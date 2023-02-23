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

	void Imgui();

	InputManager* inputManager;
	Graphics* graphics;
	class Camera* camera;

	class PlaneMesh* planeMesh;
	class NormalShader* normalShader;

	float frameRate = 0;
	float cameraSpeed = 10.f;
	float timeElapsed = 0;
};