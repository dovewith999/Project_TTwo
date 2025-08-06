#pragma once
#include "Actor/Actor.h"
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
class TetrisBlock final : public Actor
{
	RTTI_DECLARATIONS(TetrisBlock, Actor)

public:
	explicit TetrisBlock(EBlockType type = EBlockType::None, const Vector2& position = Vector2{ 0, 0 }, EBlockState state = EBlockState::Falling, Color color = Color::White);
	virtual ~TetrisBlock();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

	// 블록 조작
	bool Move(const Vector2& direction);
	bool Rotate();
	void Drop();                    // 소프트 드롭 (한 칸씩)
	void HardDrop();               // 하드 드롭 (즉시 바닥까지)
	void SetAsFixed();
	void SetAsShadow();

	// 유틸리티 함수
	bool GetBlockPixel(int x, int y) const;                    // 특정 위치의 픽셀 확인
	bool CanMoveTo(const Vector2& position, int testRotation) const;  // 이동 가능 여부 확인
	void SetDropSpeed(float speed);                            // 낙하 속도 설정

	// 정적 함수
	static EBlockType GetRandomBlockType();                    // 랜덤 블록 타입 생성

public: /* Getter & Setter */
	FORCEINLINE EBlockType GetBlockType() const { return type; }
	FORCEINLINE EBlockState GetBlockState() const { return state; }
	FORCEINLINE Vector2 GetGridPosition() const { return gridPosition; }
	FORCEINLINE int GetRotation() const { return rotation; }
	FORCEINLINE float GetDropSpeed() const { return dropSpeed; }
	FORCEINLINE int GetBoardMarker() const { return boardMarker; }

	FORCEINLINE void SetGridPosition(const Vector2& pos) { gridPosition = pos; }
	FORCEINLINE void SetRotation(int rot) { rotation = rot % 4; }
	FORCEINLINE void SetBlockType(EBlockType newType) { type = newType; }

private:
	EBlockType type = EBlockType::None;
	EBlockState state = EBlockState::Falling;
	Vector2 gridPosition;
	int rotation = 0;
	float dropTimer = 0.0f;
	float dropSpeed = 1.0f;

	int boardMarker = 0;

};