#pragma once
#include "Engine.h"

class Level;
class Game : public Engine
{
private:
    static Game* instance;

public:
    explicit Game();
    virtual ~Game();

public:
    // 게임 특화 기능만 남김
    void StartSinglePlayer();
    void StartMultiPlayer();
    void ShowSettings();
    void QuitGame();

    // 엔진 오버라이드
    virtual void CleanUp() override;
    static Game& GetInstance();

private:
    // 레벨 관리는 LevelManager에게 위임
    void InitializeLevels();  // 시작시 모든 레벨 등록
};