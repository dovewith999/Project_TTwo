#include "LevelManager.h"
#include "Level/Level.h"

LevelManager::LevelManager()
{
}

void LevelManager::ChangeLevel(const char* nameOfLevel)
{
	Level* nextLevel = cachedLevels[nameOfLevel];
	if (nextLevel == nullptr)
	{
		return;
	}

	currentLevel = nextLevel;
	//currentLevel->BeginPlay();
}

void LevelManager::PushLevel(Level* overlayLevel)
{
}

void LevelManager::PopLevel()
{
}

void LevelManager::RegisterLevel(const std::string& name, Level* level)
{
	if (level == nullptr)
	{
		return;
	}

	cachedLevels.emplace(name, level);
}

void LevelManager::UnregisterLevel(const std::string& name)
{
}

void LevelManager::UpdateCurrentLevel(float deltaTime)
{
}

void LevelManager::RenderCurrentLevel()
{
}
