#pragma once
#include "Core.h"
#include "Math/Vector2.h"
#include "Actor/TetrisBlock.h"

/// <summary>
/// TetrisLevel과 TetrisController 사이의 의존성을 주입하기 위해 제작한 인터페이스
/// 작성자 : 임희섭
/// 작성일 : 25/08/05
/// </summary>

//__inferface
class ITetrisGameLogic abstract
{
public:
	virtual ~ITetrisGameLogic() = default;

public:
	virtual bool CanBlockMoveTo(const Vector2& position, EBlockType blockType, int rotation) const PURE;
	virtual void PlaceBlockOnBoard(TetrisBlock* block) PURE;
	virtual int ClearCompletedLines() PURE;
	virtual void ProcessCompletedLines() PURE;
	virtual void SpawnNewBlock() PURE;
	virtual void SwitchBlock() PURE;
};