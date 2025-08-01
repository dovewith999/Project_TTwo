#include "Game.h"
#include "Level/MenuLevel.h"
#include "Level/SokobanLevel.h"

Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;
	
	// 메인 레벨 추가
	AddLevel(new SokobanLevel());

	// 메뉴 레벨 생성
	menuLevel = new MenuLevel();
}

Game::~Game()
{
	CleanUp();
}

void Game::ToggleMenu()
{
	// 화면 정리
	system("cls");

	showMenu = !showMenu;

	if (showMenu)
	{
		// 게임 레벨을 뒤로 밀기
		backLevel = mainLevel;
		mainLevel = menuLevel;
	}

	else
	{
		mainLevel = backLevel;
	}
}

void Game::CleanUp()
{
	if (showMenu) // 이 때는 Engine의 mainLevel == menuLevel
	{
		SafeDelete(backLevel);
		mainLevel = nullptr;
	}
	//else // mainLevel == SokobanLevel, 이 때는 하던대로 정리하면 됨
	//{
	//}

	SafeDelete(menuLevel);

	Engine::CleanUp();
}

Game& Game::GetInstance()
{
	return *instance;
}
