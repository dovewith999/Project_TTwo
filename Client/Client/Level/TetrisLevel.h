#pragma once
#include "Level/Level.h"

class TetrisLevel final : public Level
{
	RTTI_DECLARATIONS(TetrisLevel, Level)

public:
	explicit TetrisLevel();
	virtual ~TetrisLevel();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

private:
	bool isGamePaused = false;
	bool isGameOver = false;

	float gameTimer = 0.f;
};