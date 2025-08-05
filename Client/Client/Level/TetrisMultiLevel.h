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

private:
    // 상대방 보드 관리 함수들
    void InitializeOpponentBoard();
    void UpdateOpponentBoard(const TMCPBlockData& blockData);
    void ClearOpponentCurrentBlock();
    void RenderOpponentBoard();
    void RenderMultiUI();

    // TetrisMultiLevel.h에 추가할 멤버 변수와 함수들:
private:
    // 상대방 보드 데이터
    static const int BOARD_WIDTH = 12;
    static const int BOARD_HEIGHT = 21;
    int opponentBoard[BOARD_HEIGHT][BOARD_WIDTH];

    // NetworkManager의 ReceiveThread에서 호출할 수 있도록 
    // static 함수나 friend 함수로 만들거나, 
    // NetworkManager를 수정해서 콜백 등록 방식 사용 필요

};

