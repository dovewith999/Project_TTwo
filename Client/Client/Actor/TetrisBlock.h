#pragma once
#include "Actor/Actor.h"

enum class EBlockType
{
	I, O, T, S, Z, J, L, None
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
	explicit TetrisBlock(EBlockType type = EBlockType::None, const Vector2& position = Vector2{ 0, 0 }, EBlockState state = EBlockState::Falling);
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
	Vector2 GetShadowPosition() const;                         // 그림자 위치 계산
	void SetDropSpeed(float speed);                            // 낙하 속도 설정

	// 정적 함수
	static EBlockType GetRandomBlockType();                    // 랜덤 블록 타입 생성

public: /* Getter & Setter */
	FORCEINLINE EBlockType GetBlockType() const { return type; }
	FORCEINLINE EBlockState GetBlockState() const { return state; }
	FORCEINLINE Vector2 GetGridPosition() const { return gridPosition; }
	FORCEINLINE int GetRotation() const { return rotation; }
	FORCEINLINE float GetDropSpeed() const { return dropSpeed; }

	FORCEINLINE void SetGridPosition(const Vector2& pos) { gridPosition = pos; }
	FORCEINLINE void SetRotation(int rot) { rotation = rot % 4; }
	FORCEINLINE void SetBlockType(EBlockType newType) { type = newType; }

private:
	EBlockType type = EBlockType::None;
	EBlockState state = EBlockState::Falling;   // 기존 stste 오타 수정
	Vector2 gridPosition;
	int rotation = 0;
	float dropTimer = 0.0f;
	float dropSpeed = 1.0f;

	// 주의: 블록 모양 데이터는 이제 ResourceManager에서 관리됨
};