#include "Game.h"
#include "Managers/LevelManager.h"
#include "Level/TitleLevel.h"
#include "Level/TetrisLevel.h"

Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;
	InitializeLevels();
}

Game::~Game()
{
	CleanUp();
}

void Game::StartSinglePlayer()
{
    LevelManager::GetInstance()->ChangeLevel("Tetris");
}

void Game::StartMultiPlayer()
{
}

void Game::ShowSettings()
{
}

void Game::QuitGame()
{
}

void Game::CleanUp()
{
	Engine::CleanUp();
}

Game& Game::GetInstance()
{
	return *instance;
}

void Game::InitializeLevels()
{
    auto levelManager = LevelManager::GetInstance();

    // 레벨들을 미리 생성하고 등록
    levelManager->RegisterLevel("Title", new TitleLevel());
    levelManager->RegisterLevel("Tetris", new TetrisLevel());
    //levelManager.RegisterLevel("Lobby", new LobbyLevel());
    //levelManager.RegisterLevel("Result", new ResultLevel());
    //levelManager.RegisterLevel("Settings", new SettingsLevel());

    // 시작 레벨 설정
    levelManager->ChangeLevel("Title");
}
