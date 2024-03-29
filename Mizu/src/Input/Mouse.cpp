#include <mzpch.h>
#include <Input/Mouse.h>

void Mouse::OnLeftPressed()
{
	LeftMouseDown = true;
}

void Mouse::OnLeftHold()
{
	LeftMouseDown = true;
}

void Mouse::OnLeftReleased()
{
	LeftMouseDown = false;
}

void Mouse::OnMiddlePressed()
{
	MiddleMouseDown = true;
}

void Mouse::OnMiddleHold()
{
	MiddleMouseDown = true;
}

void Mouse::OnMiddleReleased()
{
	MiddleMouseDown = false;
}

void Mouse::OnRightPressed()
{
	RightMouseDown = true;
}

void Mouse::OnRightHold()
{
	RightMouseDown = true;
}

void Mouse::OnRightReleased()
{
	RightMouseDown = false;
}

void Mouse::OnMouseMove(int x, int y)
{
	xPos = x;
	yPos = y;
	bMouseMoving = true;
}

bool Mouse::IsLeftDown()
{
	return LeftMouseDown;
}

bool Mouse::IsMiddleDown()
{
	return MiddleMouseDown;
}

bool Mouse::IsRightDown()
{
	return RightMouseDown;
}

bool Mouse::IsMouseMoving()
{
	if (bMouseMoving)
	{
		bMouseMoving = false;
		return true;
	}

	return false;
}

float Mouse::GetMouseX()
{
	return xPos;
}

float Mouse::GetMouseY()
{
	return yPos;
}
