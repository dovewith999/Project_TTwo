#include "LevelManager.h"
#include "Level/Level.h"

LevelManager::LevelManager()
{
}

LevelManager::~LevelManager()
{	
	for (auto& iter : cachedLevels)
	{
		if (iter.second == nullptr)
		{
			continue;
		}

		// currentLevel이 해당 레벨을 가리치면 nullptr로 만들어준다. (댕글링포인터 방지)
		if (currentLevel == iter.second)
		{
			currentLevel = nullptr;
		}
		// nextLevel이 해당 레벨을 가리치면 nullptr로 만들어준다. (댕글링포인터 방지)
		if (nextLevel == iter.second)
		{
			nextLevel = nullptr;
		}

		SafeDelete(iter.second);
	}

	// 이미 null을 가리킬 것이지만 혹시 몰라 한번 더
	SafeDelete(currentLevel);
	SafeDelete(nextLevel);
}

void LevelManager::ChangeLevel(Define::ELevel nameOfLevel)
{
	if (cachedLevels[nameOfLevel] == nullptr)
	{
		return;
	}
	
	if (currentLevel != nullptr)
	{
		currentLevel->Exit();
	}

	currentLevel = cachedLevels[nameOfLevel];
}

void LevelManager::RegisterLevel(Define::ELevel nameOfLevel, Level* level)
{
	if (level == nullptr)
	{
		return;
	}

	cachedLevels.emplace(nameOfLevel, level);
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

