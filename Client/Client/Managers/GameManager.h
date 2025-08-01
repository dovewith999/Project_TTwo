#pragma once
#include "Level/Level.h"
#include "Etc/Singleton.h"

/// <summary>
/// 게임의 전반적인 상태, 상황을 관리하는 매니저 클래스
/// 게임 시작, 종료 등을 관리한다. 
/// 작성자 : 임희섭
/// 작성일 : 2025/08/01
/// </summary>
class Level;
class GameManager : public SafeSingleton<GameManager>
{
	friend class SafeSingleton<GameManager>;

public:
	explicit GameManager(Level* level);
	~GameManager() = default;

public:
	void GameStart();
	void GameExit();

private:
	// TODO : 레벨 매니저로 빼야하나.
	Level* currentLevel = nullptr; // 현재 레벨
	Level* nextLevel = nullptr; // 다음에 넘어갈 레벨
};