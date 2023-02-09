#pragma once



class Keyboard
{
public:
	Keyboard();

	void OnPressed(int i);
	void OnReleased(int i);
	void OnHold(int i);

	bool IsKeyDown(int i);

private:
	bool buttonsDown[256];
};