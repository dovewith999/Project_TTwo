#pragma once
#include "Actor/Actor.h"

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