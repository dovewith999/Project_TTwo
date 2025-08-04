#include "LevelManager.h"
#include "Level/Level.h"

LevelManager::LevelManager()
{
}

LevelManager::~LevelManager()
{
	SafeDelete(currentLevel);
	SafeDelete(nextLevel);
	
	for (auto& iter : cachedLevels)
	{
		SafeDelete(iter.second);
	}
}

void LevelManager::ChangeLevel(const char* nameOfLevel)
{
	Level* nextLevel = cachedLevels[nameOfLevel];
	if (nextLevel == nullptr)
	{
		return;
	}

	currentLevel = nextLevel;
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

void LevelManager::BeginPlayCurrentLevel()
{
	if (currentLevel == nullptr)
	{
		return;
	}

	currentLevel->BeginPlay();

}

void LevelManager::TickCurrentLevel(float deltaTime)
{
	if (currentLevel == nullptr)
	{
		return;
	}

	currentLevel->Tick(deltaTime);
}

void LevelManager::RenderCurrentLevel()
{
	if (currentLevel == nullptr)
	{
		return;
	}

	currentLevel->Render();
}

