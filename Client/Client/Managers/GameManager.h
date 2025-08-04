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
class GameManager final : public SafeSingleton<GameManager>
{
	friend class SafeSingleton<GameManager>;

public:
	explicit GameManager(Level* level);
	virtual ~GameManager() = default;

public:
	void GameStart();
	void GameExit();
};