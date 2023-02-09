#include <mzpch.h>
#include <Input/InputManager.h>

#include <Input/Mouse.h>

InputManager::InputManager()
{
	mouse = new Mouse();
}

InputManager::~InputManager()
{
	
}