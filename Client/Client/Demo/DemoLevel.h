#pragma once
#include "Level/Level.h"

class DemoLevel : public Level
{
	RTTI_DECLARATIONS(DemoLevel, Level)

public:
	DemoLevel();
	virtual ~DemoLevel() = default;
};