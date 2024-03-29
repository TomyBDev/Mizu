#pragma once

class Mouse
{
public:
	Mouse()
		: LeftMouseDown(false),
		MiddleMouseDown(false),
		RightMouseDown(false),
		xPos(0), yPos(0) {}

	void OnLeftPressed();
	void OnLeftHold();
	void OnLeftReleased();

	void OnMiddlePressed();
	void OnMiddleHold();
	void OnMiddleReleased();

	void OnRightPressed();
	void OnRightHold();
	void OnRightReleased();

	void OnMouseMove(int x, int y);

	bool IsLeftDown();
	bool IsMiddleDown();
	bool IsRightDown();

	bool IsMouseMoving();

	float GetMouseX();
	float GetMouseY();

private:
	bool LeftMouseDown;
	bool MiddleMouseDown;
	bool RightMouseDown;
	bool bMouseMoving;
	int xPos, yPos;
};