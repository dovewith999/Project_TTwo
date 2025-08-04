#pragma once
#include "Etc/Singleton.h"
#include <stack>
#include <unordered_map>
#include "Core.h"

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
    virtual ~LevelManager() = default;

public:
    // 레벨 전환
    void ChangeLevel(const char* nameOfLevel);
    void PushLevel(Level* overlayLevel);    // 메뉴 오버레이
    void PopLevel();                        // 메뉴 닫기

    // 레벨 생명주기 관리
    void RegisterLevel(const std::string& name, Level* level);
    void UnregisterLevel(const std::string& name);

    // Engine과의 연동
    FORCEINLINE Level* GetCurrentLevel() const { return currentLevel; }
    void UpdateCurrentLevel(float deltaTime);

private:
    std::stack<Level*> levelStack;     // 레벨 스택 (메뉴 오버레이용)
    Level* currentLevel = nullptr;      // 현재 활성 레벨
    Level* nextLevel = nullptr;         // 전환 대기 레벨

    // 미리 생성된 레벨들 (메모리 풀링)
    std::unordered_map<std::string, Level*> cachedLevels;
};

