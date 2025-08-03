#include "TetrisLevel.h"
#include "Utils/Define.h"
#include "Managers/SoundManager.h"
#include "Managers/FileManager.h"

TetrisLevel::TetrisLevel()
{
}

TetrisLevel::~TetrisLevel()
{
}

void TetrisLevel::BeginPlay()
{
	super::BeginPlay();

	if (calledBeginPlay == true)
	{
		return;
	}

	calledBeginPlay = true;
	system("cls");
	//SoundManager::GetInstance()->PlaySoundW(L"BGM Tetris_Nintendo music.mp3", Define::ESoundChannelID::BGM, 10.f);

	FileManager::GetInstance()->ReadFile("Map", Define::EFileType::MAP);
}

void TetrisLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void TetrisLevel::Render()
{
	super::Render();

}
