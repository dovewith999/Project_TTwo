#pragma once
#include "Actor/Actor.h"

enum class EBlockType
{
	I, O, T, S, Z, J, L, None,
};

enum class EBlockState
{
	Falling,
	Fixed,
	Shadow,
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

	bool Move(const Vector2& direction);
	bool Rotate();
	void Drop();
	void SetAsFixed();
	void SetAsShadow();

public: /*Getter & Setter*/
	FORCEINLINE EBlockType GetBlockType() const { return type; }

private:
	EBlockType type = EBlockType::None;
	EBlockState stste = EBlockState::Falling;
	Vector2 gridPosition;
	int rotation = 0;
	static const bool blockShapes[7][4][4][4];
	float dropTimer = 0.f;
	float dropSpeed = 0.f;
};

