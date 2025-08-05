#include "TetrisBlock.h"
#include "Managers/ResourceManager.h"  // ResourceManager 포함
#include "Utils/Utils.h"
#include <iostream>
#include <algorithm>

TetrisBlock::TetrisBlock(EBlockType type, const Vector2& position, EBlockState state, Color color)
	: type(type), gridPosition(position), state(state), rotation(0), dropTimer(0.0f) 
{
	// 레벨에 따른 기본 낙하 속도 설정 (1초 = 1000ms)
	dropSpeed = 1.0f; // 1초마다 한 칸씩 떨어짐

	super::color = color;
}

TetrisBlock::~TetrisBlock()
{
}

void TetrisBlock::BeginPlay()
{
	super::BeginPlay();

	// 초기 위치 설정 (테트리스 보드 중앙 상단)
	if (gridPosition.x == 0 && gridPosition.y == 0)
	{
		gridPosition.x = 4; // 보드 중앙 (10칸 기준)
		gridPosition.y = 0; // 맨 위
	}
}

void TetrisBlock::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// 떨어지는 블록인 경우에만 자동 낙하
	if (state == EBlockState::Falling)
	{
		dropTimer += deltaTime;

		// 낙하 시간이 되었으면 한 칸 아래로 이동
		if (dropTimer >= dropSpeed)
		{
			Move(Vector2{ 0, 1 });
			dropTimer = 0.0f;
		}
	}
}

void TetrisBlock::Render()
{
	super::Render();

	if (type == EBlockType::None)
		return;

	// 그림자 블록은 다른 색상으로 표시
	char displayChar = (state == EBlockState::Shadow) ? '░' : '█';

	// 현재 블록 모양에 따라 렌더링
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			if (GetBlockPixel(x, y))
			{
				Vector2 screenPos = {
					gridPosition.x + x,
					gridPosition.y + y
				};

				// 화면 범위 내에서만 렌더링
				if (screenPos.x >= 0 && screenPos.x < 10 && screenPos.y >= 0 && screenPos.y < 20)
				{
					Utils::SetConsoleCursorPosition(screenPos);
					Utils::SetConsoleTextColor(color);
					std::cout << displayChar;
				}
			}
		}
	}
}

bool TetrisBlock::Move(const Vector2& direction)
{
	if (state == EBlockState::Fixed)
	{
		return false;
	}

	Vector2 newPosition = {
		gridPosition.x + direction.x,
		gridPosition.y + direction.y
	};

	// 이동 가능한지 검사
	if (CanMoveTo(newPosition, rotation))
	{
		gridPosition = newPosition;
		return true;
	}

	// 아래로 이동할 수 없다면 고정 상태로 변경
	if (direction.y > 0)
	{
		SetAsFixed();
	}

	return false;
}

bool TetrisBlock::Rotate()
{
	if (state == EBlockState::Fixed || type == EBlockType::O)
		return false; // O블록은 회전하지 않음

	int newRotation = (rotation + 1) % 4;

	// 회전 가능한지 검사
	if (CanMoveTo(gridPosition, newRotation))
	{
		rotation = newRotation;
		return true;
	}

	// Wall Kick 시도 (간단한 버전)
	Vector2 kickOffsets[] = { {-1, 0}, {1, 0}, {0, -1}, {-1, -1}, {1, -1} };

	for (const Vector2& offset : kickOffsets)
	{
		Vector2 testPos = { gridPosition.x + offset.x, gridPosition.y + offset.y };
		if (CanMoveTo(testPos, newRotation))
		{
			gridPosition = testPos;
			rotation = newRotation;
			return true;
		}
	}

	return false;
}

void TetrisBlock::Drop()
{
	if (state == EBlockState::Fixed)
		return;

	// 한 칸씩 아래로 이동
	while (Move(Vector2{ 0, 1 }))
	{
		// 이동 가능한 동안 계속 이동
	}
}

void TetrisBlock::HardDrop()
{
	if (state == EBlockState::Fixed)
		return;

	// 즉시 바닥까지 떨어뜨리고 고정
	Drop();
	SetAsFixed();
}

void TetrisBlock::SetAsFixed()
{
	state = EBlockState::Fixed;
	dropTimer = 0.0f;
}

void TetrisBlock::SetAsShadow()
{
	state = EBlockState::Shadow;
	dropTimer = 0.0f;
}

bool TetrisBlock::GetBlockPixel(int x, int y) const
{
	if (type == EBlockType::None || x < 0 || x >= 4 || y < 0 || y >= 4)
		return false;

	// ResourceManager에서 블록 모양 데이터 가져오기
	int typeIndex = static_cast<int>(type);
	const BlockShapeData* shapeData = ResourceManager::GetInstance()->GetBlockShape(typeIndex, rotation);

	if (shapeData)
	{
		return shapeData->GetPixel(x, y);
	}

	// ResourceManager에서 데이터를 가져올 수 없는 경우 기본값 반환
	std::cout << "[TetrisBlock] 경고: ResourceManager에서 블록 데이터를 가져올 수 없음\n";
	return false;
}

bool TetrisBlock::CanMoveTo(const Vector2& position, int testRotation) const
{
	// 테트리스 보드 경계 검사 및 충돌 검사
	// 실제 게임에서는 TetrisLevel이나 GameManager에서 보드 상태를 확인해야 함

	int typeIndex = static_cast<int>(type);
	const BlockShapeData* shapeData = ResourceManager::GetInstance()->GetBlockShape(typeIndex, testRotation);

	if (!shapeData)
		return false;

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			if (shapeData->GetPixel(x, y))
			{
				int worldX = position.x + x;
				int worldY = position.y + y;

				// 보드 경계 검사
				if (worldX < 0 || worldX >= 10 || worldY >= 20)
					return false;

				// 위쪽 경계는 허용 (블록이 위에서 시작하므로)
				if (worldY < 0)
					continue;

			}
		}
	}

	return true;
} 

void TetrisBlock::SetDropSpeed(float speed)
{
	dropSpeed = (speed < 0.1f) ? 0.1f : speed;
}

EBlockType TetrisBlock::GetRandomBlockType()
{
	static int blockTypes[] = { 0, 1, 2, 3, 4, 5, 6 }; // I, O, T, S, Z, J, L
	int randomIndex = rand() % 7;
	return static_cast<EBlockType>(blockTypes[randomIndex]);
}