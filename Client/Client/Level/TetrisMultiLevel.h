#pragma once
#include "TetrisLevel.h"
#include "Managers/NetworkManager.h"

/// <summary>
/// 멀티 플레이 환경을 담당하는 레벨 클래스
/// 본인의 플레이 환경을 관리하는 TetrisLevel의 기능을 받기 위해 상속 받음
/// 추가로 상대방의 보드를 그려주는 역할을 담당
/// 작성자 : 임희섭
/// 작성일 : 25/08/05
/// </summary>
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
    virtual int ClearCompletedLines() override;
    void ReceiveAttackFromOpponent(int attackLines); 
    void AddAttackLinesToOpponentBoard(int attackLines);

public:
    void UpdateOpponentBoard(const TMCPBlockData& blockData);
    void AddAttackLinesToBottom(int lines); 
    
private:
    // 상대방 보드 관리 함수들
    void InitializeOpponentBoard();
    void ClearOpponentCurrentBlock();
    void RenderWaitingScreen(); 
    void RenderOpponentBoard();
    void RenderMultiUI();
    void ClearOpponentCompletedLines();

private:
    // 상대방 보드 데이터
    int opponentBoard[BOARD_HEIGHT][BOARD_WIDTH] = {};
};

