#include "../Game/Game.h"
#include "Utils/Utils.h"
#include "Manager/SoundManager.h"
#include "TitleLevel.h"
#include "Input.h"

#include <iostream>

TitleLevel::TitleLevel()
{	
	// 메뉴 아이템 추가  
	items.emplace_back(new TitleItem("- Start Game", []() {Game::GetInstance().StartSinglePlayer(); }));
	items.emplace_back(new TitleItem("- Quit Game", []() { Game::GetInstance().Quit(); }));

	length = static_cast<int>(items.size());

	// 음악 재생
	//SoundManager::GetInstance()->PlaySoundW(L"BGM Tetris_Nintendo music.mp3", Define::ESoundChannelID::BGM, 10.f);
}

TitleLevel::~TitleLevel()
{
	for (TitleItem* item : items)
	{
		SafeDelete(item);
	}

	items.clear();
}

void TitleLevel::Tick(float deltaTime)
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
#pragma endregion
}

void TitleLevel::Render()
{
	super::Render();

	// 색상 & 좌표 정리
	Utils::SetConsoleCursorPosition(Vector2{ 0,0 });
	Utils::SetConsoleTextColor(static_cast<WORD>(unselectedColor));

	std::cout << "TETRIS\n\n";

	for (int i = 0; i < length; ++i)
	{
		Color textColor = i == currentIndex ? selectedColor : unselectedColor;
		items[i]->itemText[0] = i == currentIndex ? selectImage : '-';
		Utils::SetConsoleTextColor(static_cast<WORD>(textColor));
		std::wcout << items[i]->itemText << '\n';
	}
}