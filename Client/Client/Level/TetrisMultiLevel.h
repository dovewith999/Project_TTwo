#pragma once
#include "TetrisLevel.h"

class TetrisMultiLevel final : public TetrisLevel
{
	RTTI_DECLARATIONS(TetrisMultiLevel, TetrisLevel)

public:
	explicit TetrisMultiLevel() = default;
	virtual ~TetrisMultiLevel() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

private:
	bool isGamePaused = false;
	bool isGameOver = false;

	float gameTimer = 0.f;

};

