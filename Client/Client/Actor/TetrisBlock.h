#pragma once
#include "Core.h"
#include "Math/Vector2.h"
#include "Math/Color.h"

/// <summary>
/// 테트리스의 블럭 역할을 하는 클래스
/// 그런데 굳이 필요가 없을 것 같다.
/// 블럭 타입, gridPosition, rotationState 말고 필요있는 데이터가 딱히 없다.
/// 최종 리팩토링 때 많이 쳐내질 예정
/// 작성자 : 임희섭
/// 작성일 : 25/08/05
/// </summary>
enum class EBlockType
{
	I = 2, O, T, S, Z, J, L, None
};

enum class EBlockState
{
	Falling,
	Fixed,
	Shadow
};

class TetrisLevel;
class TetrisBlock
{

public:
	explicit TetrisBlock(EBlockType type = EBlockType::None, const Vector2& position = Vector2{ 0, 0 }, EBlockState state = EBlockState::Falling, Color color = Color::White);
	~TetrisBlock();

public: /* Getter & Setter */
	FORCEINLINE EBlockType GetBlockType() const { return type; }
	FORCEINLINE EBlockState GetBlockState() const { return state; }
	FORCEINLINE Vector2 GetGridPosition() const { return gridPosition; }
	FORCEINLINE int GetRotation() const { return rotation; }
	FORCEINLINE float GetDropSpeed() const { return dropSpeed; }
	FORCEINLINE int GetBoardMarker() const { return boardMarker; }
	FORCEINLINE Color GetColor() const { return color; }

	FORCEINLINE void SetGridPosition(const Vector2& pos) { gridPosition = pos; }
	FORCEINLINE void SetRotation(int rot) { rotation = rot % 4; }
	FORCEINLINE void SetBlockType(EBlockType newType) { type = newType; }

private:
	int rotation = 0;
	int boardMarker = 0;
	float dropTimer = 0.0f;
	float dropSpeed = 1.0f;
	Vector2 gridPosition;
	Color color = Color::White;
	EBlockType type = EBlockType::None;
	EBlockState state = EBlockState::Falling;

};