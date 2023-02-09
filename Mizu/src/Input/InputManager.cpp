#include <mzpch.h>
#include <Input/InputManager.h>

#include <Input/Mouse.h>
#include <Input/Keyboard.h>

InputManager::InputManager()
{
	mouse = new Mouse();
	keyboard = new Keyboard();
}

InputManager::~InputManager()
{
	
}