#include "TetrisBlock.h"
#include "Managers/ResourceManager.h"  // ResourceManager 포함
#include "Utils/Utils.h"
#include <iostream>
#include <algorithm>

TetrisBlock::TetrisBlock(EBlockType type, const Vector2& position, EBlockState state, Color color)
	: type(type), gridPosition(position), state(state), rotation(0), dropTimer(0.0f), color(color)
{
	// 레벨에 따른 기본 낙하 속도 설정 (1초 = 1000ms)
	dropSpeed = 1.0f; // 1초마다 한 칸씩 떨어짐
	boardMarker = static_cast<int>(type);
}

TetrisBlock::~TetrisBlock()
{
}
