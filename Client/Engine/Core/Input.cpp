#include "Input.h"
#include <Windows.h>

Input* Input::instance = nullptr;

Input::Input()
{
	instance = this;
}

bool Input::GetKey(int keyCode) const
{
	return keyStates[keyCode].isKeyDown;
}

bool Input::GetKeyDown(int keyCode) const
{
	return !keyStates[keyCode].previousKeyDown && keyStates[keyCode].isKeyDown;
}

bool Input::GetKeyUp(int keyCode) const
{
	return keyStates[keyCode].previousKeyDown && !keyStates[keyCode].isKeyDown;
}

void Input::ProcessInput()
{
	// 키 입력 확인
	for (int i = 0; i < MAX_KEY_NUMBER; ++i)
	{
		keyStates[i].isKeyDown = GetAsyncKeyState(i) & 0x8000;
	}
}

void Input::SavePreviouseKeyStates()
{
	// 현재 프레임의 입력을 기록
	for (int i = 0; i < MAX_KEY_NUMBER; ++i)
	{
		keyStates[i].previousKeyDown = keyStates[i].isKeyDown;
	}
}

Input& Input::GetInstance()
{
	return *instance;
}