#pragma once
#include "TetrisLevel.h"
#include "Managers/NetworkManager.h"

class TetrisMultiLevel final : public TetrisLevel
{
	RTTI_DECLARATIONS(TetrisMultiLevel, TetrisLevel)

public:
	explicit TetrisMultiLevel() = default;
	virtual ~TetrisMultiLevel() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;
    virtual void Exit() override;

public:
    virtual void EndGame() override;

public:
    void UpdateOpponentBoard(const TMCPBlockData& blockData);

private:
    // 상대방 보드 관리 함수들
    void InitializeOpponentBoard();
    void ClearOpponentCurrentBlock();
    void RenderOpponentBoard();
    void RenderMultiUI();
    void ClearOpponentCompletedLines();

private:
    // 상대방 보드 데이터
    //static const int BOARD_WIDTH = 12;
    //static const int BOARD_HEIGHT = 21;
    int opponentBoard[BOARD_HEIGHT][BOARD_WIDTH] = {};

    //bool calledBeginPlay_multiLevel = false;
};

