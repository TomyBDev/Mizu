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
	void HandleInput();

	void Imgui();

	InputManager* inputManager;
	Graphics* graphics;

	class TriangleMesh* triangleMesh;
	class NormalShader* normalShader;

	float frameRate = 0;
};