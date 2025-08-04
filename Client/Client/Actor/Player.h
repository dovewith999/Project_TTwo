#pragma once
#include "Actor/Actor.h"
/// <summary>
/// 게임을 플레이하는 기능을 가진 클래스
/// 여기서 입력을 받고 게임에 직접적인 영향을 끼칠 예정
/// 작성자 : 임희섭
/// 작성일 : 25/08/04
/// </summary>

class Player final : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	Player(const Vector2& position);
	virtual ~Player();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	class ICanPlayerMove* canPlayerMoveInterface = nullptr;
};