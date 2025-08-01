#pragma once
#include "Engine.h"

class Level;
class Game : public Engine
{
public:
	Game();
	virtual ~Game();

public:
	void ToggleMenu();

	virtual void CleanUp() override;

public:
	static Game& GetInstance();

private:
	// 메뉴 레벨
	Level* menuLevel = nullptr;

	// 화면에 안보이는 레벨
	Level* backLevel = nullptr;

	bool showMenu = false;

private:
	static Game* instance;
};