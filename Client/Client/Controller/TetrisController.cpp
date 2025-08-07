#include "TetrisController.h"
#include "Input.h"
#include "Actor/TetrisBlock.h"
#include "Managers/NetworkManager.h"
#include "Level/TetrisMultiLevel.h"
#include "Managers/LevelManager.h"
#include "Managers/SoundManager.h"

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
			SendInput(VK_LEFT); //현재 고정된 블록 정보
			SoundManager::GetInstance()->PlaySoundW(L"MOVE.wav", Define::ESoundChannelID::EFFECT, 3.f);
		}
	}
	else if (Input::GetInstance().GetKeyDown(VK_RIGHT))
	{
		Vector2 newPos = { currentPos.x + 1, currentPos.y };
		if (gameLogic->CanBlockMoveTo(newPos, currentControlBlock->GetBlockType(), currentControlBlock->GetRotation()))
		{
			currentControlBlock->SetGridPosition(newPos);
			SendInput(VK_RIGHT); //현재 고정된 블록 정보
			SoundManager::GetInstance()->PlaySoundW(L"MOVE.wav", Define::ESoundChannelID::EFFECT, 3.f);
		}
	}

	// 회전
	else if (Input::GetInstance().GetKeyDown(VK_UP))
	{
		int newRotation = (currentControlBlock->GetRotation() + 1) % 4;
		if (gameLogic->CanBlockMoveTo(currentPos, currentControlBlock->GetBlockType(), newRotation))
		{
			currentControlBlock->SetRotation(newRotation);
			SendInput(VK_UP); //현재 블록 정보
			SoundManager::GetInstance()->PlaySoundW(L"ROTATE.wav", Define::ESoundChannelID::EFFECT, 3.f);
		}
	}

	// 소프트 드롭
	else if (Input::GetInstance().GetKeyDown(VK_DOWN))
	{
		Vector2 newPos = { currentPos.x, currentPos.y + 1 };
		if (gameLogic->CanBlockMoveTo(newPos, currentControlBlock->GetBlockType(), currentControlBlock->GetRotation()))
		{
			currentControlBlock->SetGridPosition(newPos);
			SendInput(VK_DOWN);
			SoundManager::GetInstance()->PlaySoundW(L"SOFTDROP.wav", Define::ESoundChannelID::EFFECT, 3.f);

		}
		else
		{
			// 더 이상 떨어질 수 없으면 고정
			gameLogic->PlaceBlockOnBoard(currentControlBlock);
			gameLogic->ProcessCompletedLines();
			SendInput(VK_DOWN, true); //현재 고정된 블록 정보
			SoundManager::GetInstance()->PlaySoundW(L"LANDING.wav", Define::ESoundChannelID::EFFECT, 3.f);
			
			gameLogic->SpawnNewBlock();
			SendInput(VK_DOWN); //새로 스폰된 블록 정보
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
		SendInput(VK_SPACE, true); //현재 고정된 블록 정보
		SoundManager::GetInstance()->PlaySoundW(L"HARDDROP.wav", Define::ESoundChannelID::EFFECT, 3.f);

		gameLogic->SpawnNewBlock();
		SendInput(VK_SPACE); //새로 스폰된 블록 정보
	}

	// 블럭 스위칭
	else if (Input::GetInstance().GetKeyDown(VK_TAB))
	{
		gameLogic->SwitchBlock();
		SendInput(VK_TAB);
	}
}

bool TetrisController::IsMultiplayerMode() const
{
	// RTTI를 사용해서 TetrisMultiLevel인지 확인
	return LevelManager::GetInstance()->GetCurrentLevel()->As<TetrisMultiLevel>() != nullptr;
}

void TetrisController::SendInput(int input, bool isFixed)
{
	if (IsMultiplayerMode() == false)
	{
		return;
	}

	NetworkManager::GetInstance()->SendInput(currentControlBlock, input, isFixed);
}

void TetrisController::SetCurrentBlock(TetrisBlock* newBlock)
{
	currentControlBlock = newBlock;
}
