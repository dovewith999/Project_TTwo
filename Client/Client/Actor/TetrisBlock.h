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

private:
	int rotateBlock[20][4] = {}; // 회전된 모습을 가지고 있을 배열
 
};

