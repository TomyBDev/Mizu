#pragma once

using namespace DirectX;

class Camera
{
public:
	Camera(HWND hwnd);

	void HandleInput(class InputManager* input, float dt);

	void Update();

	XMMATRIX GetViewMatrix();

	void SetSpeed(float newSpeed) { speed = newSpeed; }

	const XMVECTOR GetPosition() { return position; };
	const XMVECTOR GetRotation() { return rotation; };

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
	XMVECTOR position = { 0,0,0 };
	XMVECTOR rotation = { 0,0,0 };
	XMVECTOR forward = { 0,0,0 };
	XMVECTOR up = { 0,0,0 };
	XMVECTOR right = { 0,0,0 };
	XMMATRIX viewMatrix;

	POINT cursor;
	HWND hWnd;

	bool bTurningEnabled;

	float speed = 10.f;
	float sensitivity = 10.f;
};