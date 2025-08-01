#include "MenuLevel.h"  
#include "Game/Game.h"  
#include "Utils/Utils.h"
#include <iostream>
#include "Input.h"

MenuLevel::MenuLevel()
{
	// 메뉴 아이템 추가  
	items.emplace_back(new MenuItem("Resume Game", []() {Game::GetInstance().ToggleMenu(); }));
	items.emplace_back(new MenuItem("Quit Game", []() { Game::GetInstance().Quit(); }));

	length = static_cast<int>(items.size());
}


MenuLevel::~MenuLevel()
{
	for (MenuItem* item : items)
	{
		SafeDelete(item);
	}

	items.clear();
}

void MenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

#pragma region 입력 처리
	if (Input::GetInstance().GetKeyDown(VK_UP))
	{
		currentIndex = (currentIndex - 1 + length) % length;
	}

	else if (Input::GetInstance().GetKeyDown(VK_DOWN))
	{
		currentIndex = (currentIndex + 1) % length;
	}

	// 엔터 키 입력
	else if (Input::GetInstance().GetKeyDown(VK_RETURN))
	{
		// 메뉴 아이템이 저장하고 있는 함수 호출
		items[currentIndex]->onSelected();
	}

	else if (Input::GetInstance().GetKeyDown(VK_ESCAPE))
	{
		// Game에 토글 메뉴 기능 추가 후 호출해야 함
		Game::GetInstance().ToggleMenu();
		currentIndex = 0;
	}
#pragma endregion
}

void MenuLevel::Render()
{
	super::Render();

	// 색상 & 과표 정리
	Utils::SetConsoleCursorPosition({ 0,0 });
	Utils::SetConsoleTextColor(static_cast<WORD>(unselectedColor));

	std::cout << "SokobanGame\n\n";

	for (int i = 0; i < length; ++i)
	{
		Color textColor = i == currentIndex ? selectedColor : unselectedColor;
		Utils::SetConsoleTextColor(static_cast<WORD>(textColor));
		std::cout << items[i]->menuText << '\n';
	}
}
