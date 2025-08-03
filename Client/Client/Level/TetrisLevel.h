#pragma once
#include "Level/Level.h"

class TetrisLevel : public Level
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
	bool isGamePaused;
	bool isGameOver;

	float gameTimer;
};