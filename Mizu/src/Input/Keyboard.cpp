#include <mzpch.h>
#include <Input/Keyboard.h>

Keyboard::Keyboard()
{
	for (int i = 0; i < sizeof(buttonsDown); i++)
		buttonsDown[i] = false;
}

void Keyboard::OnPressed(int i)
{
	buttonsDown[i] = true;
}

void Keyboard::OnReleased(int i)
{
	buttonsDown[i] = false;
}

void Keyboard::OnHold(int i)
{
	buttonsDown[i] = true;
}

bool Keyboard::IsKeyDown(int i)
{
	return buttonsDown[i];
}
