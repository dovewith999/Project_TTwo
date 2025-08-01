#include "Actor.h"
#include "Engine.h"
#include "Utils/Utils.h"
#include "Level/Level.h"
#include <Windows.h>
#include <iostream>

Actor::Actor(const char image, Color color, const Vector2& position)
	: image(image), color(color), position(position)
{
}

Actor::~Actor()
{
}

void Actor::BeginPlay()
{
	hasBeganPlay = true;
}

void Actor::Tick(float deltaTime)
{
}

void Actor::Render()
{
	// Win32 API.
	// 커서 위치 이동

	//콘솔 출력 제어를 위한 핸들 가져오기 -> Utils 함수에 자체 제작
	//static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	//커서 포지션 이동
	COORD coord;
	coord.X = static_cast<short>(position.x);
	coord.Y = static_cast<short>(position.y);

	Utils::SetConsoleCursorPosition(coord);
	
	// 색상 설정
	Utils::SetConsoleTextColor(static_cast<WORD>(color));

	//그리기
	std::cout << image;
}

void Actor::QuitGame() const
{
	Engine::GetInstance().Quit();
}

void Actor::SetPosition(const Vector2& newPosition)
{
#pragma region 더블 버퍼링 작성하기 전 까지 사용할 꼼수
	//static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	//커서 포지션 이동
	COORD coord;
	coord.X = static_cast<short>(position.x);
	coord.Y = static_cast<short>(position.y);

	Utils::SetConsoleCursorPosition(coord);

	std::cout << ' ';
#pragma endregion

	position = newPosition;
}

Vector2 Actor::GetPosition() const
{
	return position;
}

void Actor::SetSortingOrder(unsigned int sortingOrder)
{
	this->sortingOrder = sortingOrder;
}

unsigned int Actor::GetSortringOrder() const
{
	return sortingOrder;
}

void Actor::SetOwner(Level* newOwner)
{
	owner = newOwner;
}

Level* Actor::GetOwner() const
{
	return owner;
}
