#pragma once
#include "Math/Vector2.h"

// 플레이어가 이동할 수 있는지 확인할 때 사용할 인터페이스
class ICanPlayerMove
{
public:
	virtual bool CanPlayerMove(const Vector2 & playerPosition, const Vector2 & newPosition) PURE;
};