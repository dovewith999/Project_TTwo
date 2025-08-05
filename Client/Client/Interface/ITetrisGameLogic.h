#pragma once
#include "Core.h"
#include "Math/Vector2.h"
#include "Actor/TetrisBlock.h"

class ITetrisGameLogic abstract
{
public:
	virtual ~ITetrisGameLogic() = default;

public:
	virtual bool CanBlockMoveTo(const Vector2& position, EBlockType blockType, int rotation) const PURE;
	virtual void PlaceBlockOnBoard(TetrisBlock* block) PURE;
	virtual int ClearCompletedLines() PURE;
	virtual void ProcessCompletedLines() PURE;
};