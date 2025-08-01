#pragma once
#include "Actor/Actor.h"

class Ground final : public Actor
{
	RTTI_DECLARATIONS(Ground, Actor)

public:
	Ground(const Vector2& position);
	virtual ~Ground() = default;
};