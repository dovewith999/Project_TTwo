#include "Actor.h"
#include "Engine.h"
#include "Utils/Utils.h"
#include "Level/Level.h"
#include <Windows.h>
#include <iostream>

Actor::Actor(const char* image, Color color, const Vector2& position)
	: color(color), position(position)
{
	width = static_cast<int>(strlen(image));
	this->image = new char[width + 1];
	strcpy_s(this->image, width + 1, image);

}

Actor::~Actor()
{
	SafeDeleteArray(image);
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
	Utils::SetConsoleCursorPosition(position);
	
	// 색상 설정
	Utils::SetConsoleTextColor(color);

	//그리기
	std::cout << image;
}

bool Actor::TestIntersect(const Actor* const other)
{
	// AABB : Axis Aligned Bounding Box
	// NOTE : 현재 액터 구조 상 세로는 크기가 없음 (크기가 1)
	// 따라서 가로의 최소/최대 위치만 더 고려하면 됨
	// 이 액터의 x좌표 정보.
	int minX = position.x;
	int maxX = position.x + width - 1;

	// 충돌 비교를 할 다른 액터의 x 좌표정보
	int otherMinX = other->position.x;
	int otherMaxX = other->position.x + other->width - 1;

	// 다른 액터의 왼쪽 끝이 내 오른쪽 끝보다 클 경우 절대 겹치지 않음
	if (otherMinX > maxX)
	{
		return false;
	}

	// 다른 액터의 오른쪽 끝이 내 왼쪽 끝보다 클 경우 절대 겹치지 않음
	if (otherMaxX < minX)
	{
		return false;
	}

	// y좌표가 같은지 최종 확인
	return position.y == other->position.y;
}

void Actor::Destroy()
{
	// 삭제 요청 되었다고 설정
	isExpired = true;

	owner->DestroyActor(this);
}

void Actor::QuitGame() const
{
	Engine::GetInstance().Quit();
}

void Actor::SetPosition(const Vector2& newPosition)
{
	// 예외 처리
	if (newPosition.x < 0 || // 왼쪽 가장자리가 화면 왼쪽을 벗어났는지 확인
		newPosition.x + width - 1 > Engine::GetInstance().GetWidth() || // 오른쪽 가장자리가 화면 오른쪽을 벗어났는지 확인
		newPosition.y < 0 || // 위쪽 가장자리가 화면의 위를 벗어났는지 확인
		newPosition.y - 1 > Engine::GetInstance().GetHeight()) // 화면 아래를 벗어났는지 확인
	{
		return;
	}

	if (position == newPosition)
	{
		return;
	}

	// 지울 위치 확인
	Vector2 direction = newPosition - position;
	position.x = direction.x >= 0 ? position.x : position.x + width - 1;

#pragma region 더블 버퍼링 작성하기 전 까지 사용할 꼼수

	Utils::SetConsoleCursorPosition(position);

	// 문자열 길이 고려해서 지울 위치 확인해야함
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

int Actor::GetWidth() const
{
	return width;
}

bool Actor::GetExpired() const
{
	return isExpired;
}

bool Actor::GetIsActive() const
{
	return isActive;
}
