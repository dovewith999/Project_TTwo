#pragma once
#include "Level/Level.h"

class TetrisMultiLevel final : public Level
{
	RTTI_DECLARATIONS(TetrisMultiLevel, Level)

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

