#include "Player.h"
#include "Engine.h"
#include "Input.h"
#include "Level/Level.h"
#include "Interface/ICanPlayerMove.h"
#include "Level/SokobanLevel.h"
#include "Game/Game.h"
#include <iostream>

Player::Player(const Vector2& position)
	: super('P', Color::Red, position)
{
	SetSortingOrder(3);
}

Player::~Player()
{
}

void Player::BeginPlay()
{
	super::BeginPlay();
	
	// 인터페이스 얻어오기
	if(GetOwner() == nullptr)
	{
		std::cout << "Error : Owner is null\n";
		return;
	}

	canPlayerMoveInterface = dynamic_cast<ICanPlayerMove*>(GetOwner());

	if (canPlayerMoveInterface == nullptr)
	{
		std::cout << "Error : canPlayerMoveInterface is null\n";
		return;
	}
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 입력처리
	if (Input::GetInstance().GetKeyDown(VK_ESCAPE))
	{
		Game::GetInstance().ToggleMenu();
		return;
	}
	// 이동 로직 - 이동하기 전에 이동할 위치로 갈 수 있는지 판단 후 이동.
	if (Input::GetInstance().GetKeyDown(VK_RIGHT))
	{
		Vector2 currentPosition = GetPosition();
		if (!canPlayerMoveInterface->CanPlayerMove(currentPosition, Vector2(currentPosition.x + 1, currentPosition.y)))
		{
			return;
		}

		Vector2 position = GetPosition();
		position.x += 1;
		SetPosition(position);
	}
	if (Input::GetInstance().GetKeyDown(VK_LEFT))
	{
		Vector2 currentPosition = GetPosition();
		if (!canPlayerMoveInterface->CanPlayerMove(currentPosition, Vector2(currentPosition.x - 1, currentPosition.y)))
		{
			return;
		}

		Vector2 position = GetPosition();
		position.x -= 1;
		SetPosition(position);
	}
	if (Input::GetInstance().GetKeyDown(VK_UP))
	{
		Vector2 currentPosition = GetPosition();
		if (!canPlayerMoveInterface->CanPlayerMove(currentPosition, Vector2(currentPosition.x, currentPosition.y - 1)))
		{
			return;
		}

		Vector2 position = GetPosition();
		position.y -= 1;
		SetPosition(position);
	}
	if (Input::GetInstance().GetKeyDown(VK_DOWN))
	{
		Vector2 currentPosition = GetPosition();
		if (!canPlayerMoveInterface->CanPlayerMove(currentPosition, Vector2(currentPosition.x, currentPosition.y + 1)))
		{
			return;
		}

		Vector2 position = GetPosition();
		position.y += 1;
		SetPosition(position);
	}
}
