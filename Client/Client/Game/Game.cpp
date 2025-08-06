#include "Game.h"
#include "Level/Level.h"
#include "Level/TitleLevel.h"
#include "Level/TetrisLevel.h"
#include "Level/TetrisMultiLevel.h"
#include "Managers/LevelManager.h"

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
    LevelManager::GetInstance()->ChangeLevel(Define::ELevel::TETRIS_SINGLE);
}

void Game::StartMultiPlayer()
{
    LevelManager::GetInstance()->ChangeLevel(Define::ELevel::TETRIS_MULTI);
}

void Game::ShowSettings()
{
}

void Game::QuitGame()
{
    Game::Quit();
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
    //auto levelManager = LevelManager::GetInstance();

    // 레벨들을 미리 생성하고 등록
    LevelManager::GetInstance()->RegisterLevel(Define::ELevel::TITLE, new TitleLevel());
    LevelManager::GetInstance()->RegisterLevel(Define::ELevel::TETRIS_SINGLE, new TetrisLevel());
    LevelManager::GetInstance()->RegisterLevel(Define::ELevel::TETRIS_MULTI, new TetrisMultiLevel());

    // 시작 레벨 설정
    LevelManager::GetInstance()->ChangeLevel(Define::ELevel::TITLE);
}
