#include "../Game/Game.h"
#include "Utils/Utils.h"
#include "Manager/SoundManager.h"
#include "TitleLevel.h"
#include "Input.h"
#include "Managers/NetworkManager.h"

#include <iostream>

TitleLevel::TitleLevel()
{
	// 메뉴 아이템 추가  
	items.emplace_back(new TitleItem("- Start Single Game", []() {Game::GetInstance().StartSinglePlayer(); }));
	items.emplace_back(new TitleItem("- Start Multi Game", []() {NetworkManager::GetInstance()->AcceptServer()/*.StartMultiPlayer()*/; }));
	items.emplace_back(new TitleItem("- Quit Game", []() { Game::GetInstance().Quit(); }));

	//length = static_cast<int>(items.size());

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

	Exit();
}

void TitleLevel::BeginPlay()
{
	super::BeginPlay();
	if (calledBeginPlay)
	{
		return;
	}

	calledBeginPlay = true;
	system("cls");

	SoundManager::GetInstance()->PlaySoundW(L"BGM_TITLE.wav", Define::ESoundChannelID::BGM, 3.f);

	// 메뉴 아이템 추가  
	//items.emplace_back(new TitleItem("- Start Single Game", []() {Game::GetInstance().StartSinglePlayer(); })); // 1인 플레이
	//items.emplace_back(new TitleItem("- Start Multi Game", []() {NetworkManager::GetInstance()->AcceptServer(); })); // 멀티 플레이
	//items.emplace_back(new TitleItem("- Quit Game", []() { Game::GetInstance().Quit(); })); // 겜 끄기

	length = static_cast<int>(items.size());
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

void TitleLevel::Exit()
{
	calledBeginPlay = false;
}
