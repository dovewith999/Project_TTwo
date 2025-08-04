#pragma once
#include <stack>
#include <unordered_map>
#include "Etc/Singleton.h"
#include "Core.h"
#include "Utils/Define.h"

/// <summary>
/// 레벨의 전환 등의 상태를 관리할 클래스
/// 기존 Game 클래스에서 하던  main, back 레벨 간 전환 기능을 담당
/// 작성자 : 임희섭
/// 작성일 : 25/08/01
/// </summary>
class Level;
class Engine_API LevelManager : public SafeSingleton<LevelManager>
{
	friend class SafeSingleton<LevelManager>;

public:
    explicit LevelManager();
    virtual ~LevelManager();

public:
    // 레벨 전환
    void ChangeLevel(Define::ELevel nameOfLevel);

    // 레벨 생명주기 관리
    void RegisterLevel(Define::ELevel nameOfLevel, Level* level);
    void UnregisterLevel(const std::string& name);

    // Engine과의 연동
    FORCEINLINE Level* GetCurrentLevel() const { return currentLevel; }
    void BeginPlayCurrentLevel();
    void TickCurrentLevel(float deltaTime);
    void RenderCurrentLevel();

private:
    Level* currentLevel = nullptr;      // 현재 활성 레벨
    Level* nextLevel = nullptr;         // 전환 대기 레벨

    // 미리 생성된 레벨들 (메모리 풀링)
    std::unordered_map<Define::ELevel, Level*> cachedLevels;
};

