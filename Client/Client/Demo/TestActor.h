#pragma once
#include "Actor/Actor.h"

class TestActor : public Actor
{
	RTTI_DECLARATIONS(TestActor, Actor)

public:
	TestActor();
	virtual ~TestActor() = default;

public:
	virtual void Tick(float deltaTime) override;
};

