#pragma once
#include "Interface/ITetrisGameLogic.h"
/// <summary>
/// 블럭에 입력 값을 부여하는 클래스
/// 키 입력을 통해 블럭을 움직일 수 있도록 한다.
/// 작성자 : 임희섭
/// 작성일 : 25/08/05
/// </summary>
class TetrisBlock;
class TetrisController
{
public:
	explicit TetrisController(ITetrisGameLogic* gameLogic);
	~TetrisController();

public:
	void ControllBlock();
	void SetCurrentBlock(TetrisBlock* newBlock);

private:
	bool IsMultiplayerMode() const;
	void SendInput(int input);

private:
	TetrisBlock* currentControlBlock = nullptr; // 현재 컨트롤하는 블럭
	ITetrisGameLogic* gameLogic = nullptr; // 의존성 주입을 위한 인터페이스

};

