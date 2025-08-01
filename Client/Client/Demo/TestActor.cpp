#include "TestActor.h"
#include <iostream>
#include "Engine.h"
#include <Windows.h>

TestActor::TestActor()
	: super('P', Color::Red)
{

}

void TestActor::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	//std::cout << "TestActor::Tick, FPS: " << (1.0f / deltaTime) << '\n';

	if (Input::GetInstance().GetKey(VK_RIGHT) && GetPosition().x < 40)
	{
		Vector2 position = GetPosition();
		position.x += 1;
		SetPosition(position);
	}

	if (Input::GetInstance().GetKey(VK_LEFT) && GetPosition().x > 0)
	{
		Vector2 position = GetPosition();
		position.x -= 1;
		SetPosition(position);
	}

	if (Input::GetInstance().GetKey(VK_UP) && GetPosition().y > 0)
	{
		Vector2 position = GetPosition();
		position.y -= 1;
		SetPosition(position);
	}

	if (Input::GetInstance().GetKey(VK_DOWN) && GetPosition().y < 10)
	{
		Vector2 position = GetPosition();
		position.y += 1;
		SetPosition(position);
	}

	if (Input::GetInstance().GetKeyDown(VK_ESCAPE))
	{
		Engine::GetInstance().Quit();
	}
}
