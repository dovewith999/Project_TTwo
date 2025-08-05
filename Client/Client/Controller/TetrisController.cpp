#include "TetrisController.h"
#include "Input.h"
#include "Actor/TetrisBlock.h"

TetrisController::TetrisController(ITetrisGameLogic* gameLogic)
	: gameLogic(gameLogic)
{
}

TetrisController::~TetrisController()
{
	// TetrisLevel에서 관리하기 때문에 SafeDelete가 아닌 nullptr 처리
	currentControlBlock = nullptr;
	gameLogic = nullptr;
}

void TetrisController::ControllBlock()
{
	Vector2 currentPos = currentControlBlock->GetGridPosition();

	// 좌우 이동
	if (Input::GetInstance().GetKeyDown(VK_LEFT))
	{
		Vector2 newPos = { currentPos.x - 1, currentPos.y };
		if (gameLogic->CanBlockMoveTo(newPos, currentControlBlock->GetBlockType(), currentControlBlock->GetRotation()))
		{
			currentControlBlock->SetGridPosition(newPos);
		}
	}
	else if (Input::GetInstance().GetKeyDown(VK_RIGHT))
	{
		Vector2 newPos = { currentPos.x + 1, currentPos.y };
		if (gameLogic->CanBlockMoveTo(newPos, currentControlBlock->GetBlockType(), currentControlBlock->GetRotation()))
		{
			currentControlBlock->SetGridPosition(newPos);
		}
	}

	// 회전
	else if (Input::GetInstance().GetKeyDown(VK_UP))
	{
		int newRotation = (currentControlBlock->GetRotation() + 1) % 4;
		if (gameLogic->CanBlockMoveTo(currentPos, currentControlBlock->GetBlockType(), newRotation))
		{
			currentControlBlock->SetRotation(newRotation);
		}
	}

	// 소프트 드롭
	else if (Input::GetInstance().GetKey(VK_DOWN))
	{
		Vector2 newPos = { currentPos.x, currentPos.y + 1 };
		if (gameLogic->CanBlockMoveTo(newPos, currentControlBlock->GetBlockType(), currentControlBlock->GetRotation()))
		{
			currentControlBlock->SetGridPosition(newPos);
		}
		else
		{
			// 더 이상 떨어질 수 없으면 고정
			gameLogic->PlaceBlockOnBoard(currentControlBlock);
			gameLogic->ProcessCompletedLines();
			gameLogic->SpawnNewBlock();
		}
	}

	// 하드 드롭
	else if (Input::GetInstance().GetKeyDown(VK_SPACE))
	{
		// 바닥까지 떨어뜨리기
		Vector2 dropPos = currentPos;
		while (gameLogic->CanBlockMoveTo(Vector2{ dropPos.x, dropPos.y + 1 }, currentControlBlock->GetBlockType(), currentControlBlock->GetRotation()))
		{
			dropPos.y++;
		}
		currentControlBlock->SetGridPosition(dropPos);

		gameLogic->PlaceBlockOnBoard(currentControlBlock);
		gameLogic->ProcessCompletedLines();
		gameLogic->SpawnNewBlock();
	}

}

void TetrisController::SetCurrentBlock(TetrisBlock* newBlock)
{
	currentControlBlock = newBlock;
}
