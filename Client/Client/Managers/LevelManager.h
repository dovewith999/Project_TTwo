#pragma once
#include "Etc/Singleton.h"

/// <summary>
/// 레벨의 전환 등의 상태를 관리할 클래스
/// 기존 Game 클래스에서 하던  main, back 레벨 간 전환 기능을 담당
/// 작성자 : 임희섭
/// 작성일 : 25/08/01
/// </summary>
class Level;
class LevelManager : public SafeSingleton<LevelManager>
{
	friend class SafeSingleton<LevelManager>;

public:
	explicit LevelManager();
	virtual ~LevelManager() = default;

public:
	void ChangeLevel(Level* nextLevel);

public:
};

