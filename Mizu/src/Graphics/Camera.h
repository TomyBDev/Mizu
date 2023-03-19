#pragma once

using namespace DirectX;

class Camera
{
public:
	Camera(HWND hwnd);

	void HandleInput(class InputManager* input, float dt);

	void Update();

	XMMATRIX GetViewMatrix();
	const XMMATRIX GetOrthoViewMatrix() { return orthoMatrix; }

	void SetSpeed(float newSpeed) { speed = newSpeed; }

	const XMVECTOR GetPosition() { return position; }
	const XMVECTOR GetRotation() { return rotation; }
	const XMVECTOR GetForward() { return forward; }

	//Movement
	void MoveForward(float dt);
	void MoveBackward(float dt);
	void MoveRight(float dt);
	void MoveLeft(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);

	void TurnDown(float dy, float dt);
	void TurnUp(float dy, float dt);
	void TurnRight(float dx, float dt);
	void TurnLeft(float dx, float dt);

private:
	XMVECTOR position = { 0.f,25.f,-5.f };
	XMVECTOR rotation = { 0.f,0,0 };
	XMVECTOR forward = { 0,0,0 };
	XMVECTOR up = { 0,0,0 };
	XMVECTOR right = { 0,0,0 };
	XMMATRIX viewMatrix, orthoMatrix;

	POINT cursor;
	HWND hWnd;

	bool bTurningEnabled;

	float speed = 10.f;
	float sensitivity = 12.f;
};