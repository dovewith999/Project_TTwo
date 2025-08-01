#pragma once
#include "Actor/Actor.h"
class Target final : public Actor
{
	RTTI_DECLARATIONS(Target, Actor)
public:
	Target(const Vector2& position);
	virtual ~Target() = default;
};