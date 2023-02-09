#pragma once

class Keyboard;
class Mouse;

class InputManager
{
public:
	InputManager();
	~InputManager();

	Keyboard* GetKeyboard() { return keyboard; }
	Mouse* GetMouse() { return mouse; }

private:
	Keyboard* keyboard;
	Mouse* mouse;
};