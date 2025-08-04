#include "TetrisBlock.h"

TetrisBlock::TetrisBlock(EBlockType type, const Vector2& position, EBlockState state)
{
}

TetrisBlock::~TetrisBlock()
{
}

void TetrisBlock::BeginPlay()
{
	super::BeginPlay();
}

void TetrisBlock::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void TetrisBlock::Render()
{
	super::Render();
}

bool TetrisBlock::Move(const Vector2& direction)
{
	return false;
}

bool TetrisBlock::Rotate()
{
	return false;
}

void TetrisBlock::Drop()
{
}

void TetrisBlock::SetAsFixed()
{
}

void TetrisBlock::SetAsShadow()
{
}
