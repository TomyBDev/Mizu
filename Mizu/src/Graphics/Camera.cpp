#include <mzpch.h>
#include <Graphics/Camera.h>

#include "Input/InputManager.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

Camera::Camera(HWND hwnd)
{
	bTurningEnabled = true;
	hWnd = hwnd;
	cursor.x = 1280 / 2;
	cursor.y = 720 / 2;
	ShowCursor(false);

	XMVECTOR position = XMVectorSet(0.f, 0.f, -10.f, 1.f);
	XMVECTOR lookAt = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 1.f);
	orthoMatrix = XMMatrixLookAtLH(position, lookAt, up);
}

void Camera::HandleInput(InputManager* input, float dt)
{

	if (input->GetKeyboard()->IsKeyDown(0x57))
	{
		OutputDebugString(L"W\n");
		MoveForward(dt);
	}

	if (input->GetKeyboard()->IsKeyDown(0x41))
	{
		OutputDebugString(L"A\n");
		MoveLeft(dt);
	}

	if (input->GetKeyboard()->IsKeyDown(0x53))
	{
		OutputDebugString(L"S\n");
		MoveBackward(dt);
	}

	if (input->GetKeyboard()->IsKeyDown(0x44))
	{
		OutputDebugString(L"D\n");
		MoveRight(dt);
	}

	if (input->GetKeyboard()->IsKeyDown(0x45))
	{
		OutputDebugString(L"E\n");
		MoveUp(dt);
	}

	if (input->GetKeyboard()->IsKeyDown(0x51))
	{
		OutputDebugString(L"Q\n");
		MoveDown(dt);
	}

	if (input->GetKeyboard()->IsKeyDown(0x46))
	{
		OutputDebugString(L"F\n");
		bTurningEnabled = !bTurningEnabled;

		if (bTurningEnabled)
			ShowCursor(false);
		else
			ShowCursor(true);
	}

	if (input->GetMouse()->IsMouseMoving() && bTurningEnabled)
	{
		OutputDebugString(L"MouseMoving\n");

		const float x = input->GetMouse()->GetMouseX() - (1280.f /2);
		const float y = input->GetMouse()->GetMouseY() - (720.f / 2);

		if (x < 0)
			TurnLeft(x, dt);
		if (x > 0)
			TurnRight(x, dt);
		if (y < 0)
			TurnDown(y, dt);
		if (y > 0)
			TurnUp(y, dt);
			

		
		cursor.x = 1280.f / 2;
		cursor.y = 720.f / 2;
		ClientToScreen(hWnd, &cursor);
		SetCursorPos(cursor.x, cursor.y);
	}
}

void Camera::Update()
{
	XMVECTOR lookAtPos = { 0,0,0,1 };
	up = { 0,1,0,1 };
	right = { 1, 0,0,1 };
	forward = { 0,0, 1, 1 };

	const float yaw = XMConvertToRadians(rotation.m128_f32[0]);
	const float pitch = XMConvertToRadians(rotation.m128_f32[1]);
	const float roll = XMConvertToRadians(rotation.m128_f32[2]);

	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	forward = XMVector3TransformCoord(forward, rotMat);
	right = XMVector3TransformCoord(right, rotMat);
	up = XMVector3TransformCoord(up, rotMat);

	lookAtPos = position + forward;

	//forward.m128_f32[0] = cosf(yaw);
	//forward.m128_f32[1] = -sinf(pitch);
	//forward.m128_f32[2] = cosf(pitch)*cosf(yaw);

	//right.m128_f32[0] = cosf(yaw);
	//right.m128_f32[1] = 0;
	//right.m128_f32[2] = -sinf(yaw);

	//up.m128_f32[0] = sinf(pitch) * sinf(yaw);
	//up.m128_f32[1] = cosf(pitch);
	//up.m128_f32[2] = sinf(pitch) * cosf(yaw);

	//lookAtPos = position + forward;

	viewMatrix = XMMatrixLookAtLH(position, lookAtPos, up);
}

XMMATRIX Camera::GetViewMatrix()
{
	return viewMatrix;
}

void Camera::MoveForward(float dt)
{
	position += forward * speed * dt;
}

void Camera::MoveBackward(float dt)
{
	position -= forward * speed * dt;
}

void Camera::MoveRight(float dt)
{
	position += right * speed * dt;
}

void Camera::MoveLeft(float dt)
{
	position -= right * speed * dt;
}

void Camera::MoveUp(float dt)
{
	position += up * speed * dt;
}

void Camera::MoveDown(float dt)
{
	position -= up * speed * dt;
}

void Camera::TurnDown(float dy, float dt)
{
	rotation.m128_f32[1] += dy * sensitivity * dt;

	if (rotation.m128_f32[1] < -90.f)
		rotation.m128_f32[1] = -90.f;
}

void Camera::TurnUp(float dy, float dt)
{
	rotation.m128_f32[1] += dy * sensitivity * dt;

	if (rotation.m128_f32[1] > 90.f)
		rotation.m128_f32[1] = 90.f;
}

void Camera::TurnRight(float dx, float dt)
{
	rotation.m128_f32[0] += dx * sensitivity * dt;

	if (rotation.m128_f32[0] > 360.f)
		rotation.m128_f32[0] -= 360.f;
}

void Camera::TurnLeft(float dx, float dt)
{
	rotation.m128_f32[0] += dx * sensitivity * dt;

	if (rotation.m128_f32[0] < 0.f)
		rotation.m128_f32[0] += 360.f;
}
