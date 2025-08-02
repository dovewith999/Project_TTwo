#include "TetrisLevel.h"
#include "Utils/Define.h"
#include "Managers/SoundManager.h"

TetrisLevel::TetrisLevel()
{
}

TetrisLevel::~TetrisLevel()
{
}

void TetrisLevel::BeginPlay()
{
	super::BeginPlay();
	system("cls");
	SoundManager::GetInstance()->PlaySoundW(L"BGM Tetris_Nintendo music.mp3", Define::ESoundChannelID::BGM, 10.f);
}

void TetrisLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void TetrisLevel::Render()
{
	super::Render();

}
