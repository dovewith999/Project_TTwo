#include "TetrisMultiLevel.h"
#include "Managers/FileManager.h"
#include "Managers/SoundManager.h"
#include "Managers/NetworkManager.h"

void TetrisMultiLevel::BeginPlay()
{
	super::BeginPlay();
}

void TetrisMultiLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (calledBeginPlay == true)
	{
		return;
	}

	calledBeginPlay = true;
	system("cls");
	//SoundManager::GetInstance()->PlaySoundW(L"BGM Tetris_Nintendo music.mp3", Define::ESoundChannelID::BGM, 10.f);

	FileManager::GetInstance()->ReadFile("Map", Define::EFileType::MAP);
}

void TetrisMultiLevel::Render()
{
	super::Render();
}
