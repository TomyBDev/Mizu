#pragma once

class InputManager;

class Application
{
public:
	Application(InputManager* input);
	~Application();

	void Update(float dt);
	void Render();
private:
	void HandleInput();

	InputManager* inputManager;
};