#pragma once
#include "Actor/Actor.h"

class TetrisBlock final : public Actor
{
	RTTI_DECLARATIONS(TetrisBlock, Actor)

public:
	explicit TetrisBlock();
	virtual ~TetrisBlock();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;
};

