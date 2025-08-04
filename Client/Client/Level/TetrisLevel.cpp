#include "TetrisLevel.h"
#include "Utils/Define.h"
#include "Utils/Utils.h"
#include "Core/Input.h"
#include "Managers/SoundManager.h"
#include "Managers/FileManager.h"
#include "Managers/ResourceManager.h"
#include <iostream>
#include <algorithm>
#include <random>

TetrisLevel::TetrisLevel()
{
	InitializeBoard();
	GenerateNextBag();
}

TetrisLevel::~TetrisLevel()
{
	SafeDelete(currentBlock);
	SafeDelete(shadowBlock);
}

void TetrisLevel::BeginPlay()
{
	super::BeginPlay();

	if (calledBeginPlay == true)
	{
		return;
	}

	calledBeginPlay = true;
	system("cls");

	// 리소스 로드
	ResourceManager::GetInstance()->Initialize();

	// 맵 파일 읽어서 게임 보드 초기화
	LoadMapFromFile("Map");

	// 게임 시작
	StartGame();
}

void TetrisLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!isGameStarted || isGamePaused || isGameOver)
		return;

	// 입력 처리
	HandleInput();

	// 블록 자동 낙하
	blockDropTimer += deltaTime;
	if (blockDropTimer >= blockDropInterval)
	{
		if (currentBlock && currentBlock->GetBlockState() == EBlockState::Falling)
		{
			Vector2 currentPos = currentBlock->GetGridPosition();
			Vector2 newPos = { currentPos.x, currentPos.y + 1 };

			if (CanBlockMoveTo(newPos, currentBlock->GetBlockType(), currentBlock->GetRotation()))
			{
				currentBlock->SetGridPosition(newPos);
			}
			else
			{
				// 블록이 더 이상 떨어질 수 없음 - 고정하고 새 블록 생성
				PlaceBlockOnBoard(currentBlock);
				ProcessCompletedLines();
				SpawnNewBlock();
			}
		}
		blockDropTimer = 0.0f;
	}

	// 그림자 블록 업데이트
	UpdateShadowBlock();

	// 게임 오버 체크
	if (IsGameOver())
	{
		EndGame();
	}
}

void TetrisLevel::Render()
{
	super::Render();

	if (!isGameStarted)
		return;

	RenderBoard();

	// UI 정보 출력 (기존 테트리스와 동일한 위치)
	Utils::SetConsoleCursorPosition(Vector2{ 32, 3 });
	std::cout << "Time  :  0:00"; // TODO: 실제 시간 계산

	Utils::SetConsoleCursorPosition(Vector2{ 32, 5 });
	std::cout << "Score :  " << score;

	Utils::SetConsoleCursorPosition(Vector2{ 32, 6 });
	std::cout << "Level :  " << level;

	Utils::SetConsoleCursorPosition(Vector2{ 32, 8 });
	std::cout << "Lines :  " << linesCleared;

	// 조작법 안내
	Utils::SetConsoleCursorPosition(Vector2{ 32, 12 });
	std::cout << "조작법:";
	Utils::SetConsoleCursorPosition(Vector2{ 32, 13 });
	std::cout << "←→: 이동  ↑: 회전";
	Utils::SetConsoleCursorPosition(Vector2{ 32, 14 });
	std::cout << "↓: 소프트드롭  SPACE: 하드드롭";
	Utils::SetConsoleCursorPosition(Vector2{ 32, 15 });
	std::cout << "P: 일시정지";

	if (isGamePaused)
	{
		Utils::SetConsoleCursorPosition(Vector2{ 15, 10 });
		std::cout << "PAUSED - Press P to resume";
	}

	if (isGameOver)
	{
		Utils::SetConsoleCursorPosition(Vector2{ 15, 10 });
		std::cout << "GAME OVER - Press R to restart";
	}
}

void TetrisLevel::EndPlay()
{
	EndGame();
}

void TetrisLevel::StartGame()
{
	isGameStarted = true;
	isGamePaused = false;
	isGameOver = false;

	score = 0;
	linesCleared = 0;
	level = 1;

	blockDropTimer = 0.0f;
	blockDropInterval = 1.0f;

	InitializeBoard();
	SpawnNewBlock();
}

void TetrisLevel::PauseGame()
{
	isGamePaused = !isGamePaused;
	std::cout << (isGamePaused ? "[TetrisLevel] 일시정지" : "[TetrisLevel] 재개") << "\n";
}

void TetrisLevel::ResumeGame()
{
	isGamePaused = false;
}

void TetrisLevel::EndGame()
{
	isGameStarted = false;
	isGameOver = true;

	SafeDelete(currentBlock);
	SafeDelete(shadowBlock);

	std::cout << "[TetrisLevel] 게임 종료! 최종 점수: " << score << "\n";
}

void TetrisLevel::SpawnNewBlock()
{
	// 이전 블록 정리
	SafeDelete(currentBlock);
	SafeDelete(shadowBlock);

	// 새 블록 생성
	EBlockType newBlockType = GetNextBlockType();
	Vector2 spawnPos = GetSpawnPosition();

	currentBlock = new TetrisBlock(newBlockType, spawnPos, EBlockState::Falling);
	shadowBlock = new TetrisBlock(newBlockType, spawnPos, EBlockState::Shadow);

	// 레벨에 액터로 추가
	AddActor(currentBlock);
	AddActor(shadowBlock);

	// 블록 시작
	currentBlock->BeginPlay();
	shadowBlock->BeginPlay();

	// 게임 오버 체크 (스폰 위치에 이미 블록이 있으면)
	if (!CanBlockMoveTo(spawnPos, newBlockType, 0))
	{
		isGameOver = true;
		std::cout << "[TetrisLevel] 게임 오버! 새 블록을 생성할 공간이 없습니다.\n";
	}
}

void TetrisLevel::HandleInput()
{
	if (!currentBlock || currentBlock->GetBlockState() != EBlockState::Falling)
		return;

	Vector2 currentPos = currentBlock->GetGridPosition();

	// 좌우 이동
	if (Input::GetInstance().GetKeyDown(VK_LEFT))
	{
		Vector2 newPos = { currentPos.x - 1, currentPos.y };
		if (CanBlockMoveTo(newPos, currentBlock->GetBlockType(), currentBlock->GetRotation()))
		{
			currentBlock->SetGridPosition(newPos);
		}
	}
	else if (Input::GetInstance().GetKeyDown(VK_RIGHT))
	{
		Vector2 newPos = { currentPos.x + 1, currentPos.y };
		if (CanBlockMoveTo(newPos, currentBlock->GetBlockType(), currentBlock->GetRotation()))
		{
			currentBlock->SetGridPosition(newPos);
		}
	}

	// 회전
	else if (Input::GetInstance().GetKeyDown(VK_UP))
	{
		int newRotation = (currentBlock->GetRotation() + 1) % 4;
		if (CanBlockMoveTo(currentPos, currentBlock->GetBlockType(), newRotation))
		{
			currentBlock->SetRotation(newRotation);
		}
	}

	// 소프트 드롭
	else if (Input::GetInstance().GetKey(VK_DOWN))
	{
		Vector2 newPos = { currentPos.x, currentPos.y + 1 };
		if (CanBlockMoveTo(newPos, currentBlock->GetBlockType(), currentBlock->GetRotation()))
		{
			currentBlock->SetGridPosition(newPos);
		}
		else
		{
			// 더 이상 떨어질 수 없으면 고정
			PlaceBlockOnBoard(currentBlock);
			ProcessCompletedLines();
			SpawnNewBlock();
		}
	}

	// 하드 드롭
	else if (Input::GetInstance().GetKeyDown(VK_SPACE))
	{
		// 바닥까지 떨어뜨리기
		Vector2 dropPos = currentPos;
		while (CanBlockMoveTo(Vector2{ dropPos.x, dropPos.y + 1 }, currentBlock->GetBlockType(), currentBlock->GetRotation()))
		{
			dropPos.y++;
		}
		currentBlock->SetGridPosition(dropPos);

		PlaceBlockOnBoard(currentBlock);
		ProcessCompletedLines();
		SpawnNewBlock();
	}

	// 일시정지
	else if (Input::GetInstance().GetKeyDown('P'))
	{
		PauseGame();
	}

	// 재시작 (게임 오버 시)
	else if (isGameOver && Input::GetInstance().GetKeyDown('R'))
	{
		StartGame();
	}
}

bool TetrisLevel::IsGameOver() const
{
	return isGameOver;
}

bool TetrisLevel::CanBlockMoveTo(const Vector2& position, EBlockType blockType, int rotation) const
{
	int typeIndex = static_cast<int>(blockType);
	const BlockShapeData* shapeData = ResourceManager::GetInstance()->GetBlockShape(typeIndex, rotation);

	if (!shapeData)
		return false;

	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			if (shapeData->GetPixel(x, y))
			{
				int boardX = position.x + x;
				int boardY = position.y + y;

				// 보드 경계 검사
				if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT)
					return false;

				// 위쪽은 허용 (블록이 위에서 시작)
				if (boardY < 0)
					continue;

				// 다른 블록과 충돌 검사 (0=빈공간만 이동 가능)
				if (gameBoard[boardY][boardX] != 0)
					return false;
			}
		}
	}

	return true;
}

void TetrisLevel::PlaceBlockOnBoard(TetrisBlock* block)
{
	if (!block)
		return;

	Vector2 pos = block->GetGridPosition();
	int typeIndex = static_cast<int>(block->GetBlockType());
	const BlockShapeData* shapeData = ResourceManager::GetInstance()->GetBlockShape(typeIndex, block->GetRotation());

	if (!shapeData)
		return;

	// 보드에 블록 고정 (2로 저장 - 고정된 블록)
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			if (shapeData->GetPixel(x, y))
			{
				int boardX = pos.x + x;
				int boardY = pos.y + y;

				if (boardX >= 0 && boardX < BOARD_WIDTH && boardY >= 0 && boardY < BOARD_HEIGHT)
				{
					gameBoard[boardY][boardX] = 2; // 고정된 블록으로 저장
				}
			}
		}
	}

	// 블록을 고정 상태로 변경
	block->SetAsFixed();
}

int TetrisLevel::ClearCompletedLines()
{
	int clearedLines = 0;

	// 아래에서 위로 라인 체크 (벽 제외하고 1~8열만)
	for (int y = BOARD_HEIGHT - 2; y >= 1; --y) // 바닥(20행)과 맨 위(0행) 제외
	{
		bool isLineFull = true;

		// 해당 라인이 가득 찼는지 확인 (벽 제외)
		for (int x = 1; x < BOARD_WIDTH - 1; ++x) // 양옆 벽 제외
		{
			if (gameBoard[y][x] == 0)
			{
				isLineFull = false;
				break;
			}
		}

		if (isLineFull)
		{
			// 라인 제거 - 위쪽 라인들을 아래로 이동
			for (int moveY = y; moveY > 1; --moveY) // 맨 위 벽까지만
			{
				for (int x = 1; x < BOARD_WIDTH - 1; ++x) // 양옆 벽 제외
				{
					gameBoard[moveY][x] = gameBoard[moveY - 1][x];
				}
			}

			// 맨 위 라인은 빈 공간으로 (벽 제외)
			for (int x = 1; x < BOARD_WIDTH - 1; ++x)
			{
				gameBoard[1][x] = 0;
			}

			clearedLines++;
			y++; // 같은 라인을 다시 체크 (위에서 내려온 라인이 또 가득 찰 수 있음)
		}
	}

	return clearedLines;
}

void TetrisLevel::RenderBoard()
{
	// 화면 지우기 (깜빡임 방지를 위해 필요한 부분만)
	Utils::SetConsoleCursorPosition(Vector2{ 0, 0 });

	// 게임 보드 출력 (기존 테트리스와 동일한 위치 - gotoxy(6, i + 3))
	for (int y = 0; y < BOARD_HEIGHT; ++y)
	{
		Utils::SetConsoleCursorPosition(Vector2{ 6, y + 3 });

		for (int x = 0; x < BOARD_WIDTH; ++x)
		{
			int cellValue = gameBoard[y][x];
			bool isCurrentBlock = false;
			bool isShadowBlock = false;

			// 현재 떨어지는 블록 위치 확인
			if (currentBlock && currentBlock->GetBlockState() == EBlockState::Falling)
			{
				Vector2 blockPos = currentBlock->GetGridPosition();
				int typeIndex = static_cast<int>(currentBlock->GetBlockType());
				const BlockShapeData* shapeData = ResourceManager::GetInstance()->GetBlockShape(typeIndex, currentBlock->GetRotation());

				if (shapeData)
				{
					for (int by = 0; by < 4 && !isCurrentBlock; ++by)
					{
						for (int bx = 0; bx < 4 && !isCurrentBlock; ++bx)
						{
							if (shapeData->GetPixel(bx, by))
							{
								int worldX = blockPos.x + bx;
								int worldY = blockPos.y + by;

								if (worldX == x && worldY == y)
								{
									isCurrentBlock = true;
								}
							}
						}
					}
				}
			}

			// 그림자 블록 위치 확인 (현재 블록과 겹치지 않는 경우만)
			if (!isCurrentBlock && shadowBlock)
			{
				Vector2 shadowPos = shadowBlock->GetGridPosition();
				int typeIndex = static_cast<int>(shadowBlock->GetBlockType());
				const BlockShapeData* shapeData = ResourceManager::GetInstance()->GetBlockShape(typeIndex, shadowBlock->GetRotation());

				if (shapeData)
				{
					for (int by = 0; by < 4 && !isShadowBlock; ++by)
					{
						for (int bx = 0; bx < 4 && !isShadowBlock; ++bx)
						{
							if (shapeData->GetPixel(bx, by))
							{
								int worldX = shadowPos.x + bx;
								int worldY = shadowPos.y + by;

								if (worldX == x && worldY == y && cellValue == 0)
								{
									isShadowBlock = true;
								}
							}
						}
					}
				}
			}

			// 렌더링 (기존 테트리스와 동일한 문자 사용)
			if (isCurrentBlock)
			{
				std::cout << "■"; // 현재 떨어지는 블록 (3번과 동일)
			}
			else if (isShadowBlock)
			{
				std::cout << "▨"; // 그림자 블록 (1번과 동일)
			}
			else
			{
				// Map.txt에서 읽어온 값 + 고정된 블록에 따라 렌더링
				switch (cellValue)
				{
				case 0: std::cout << "  "; break; // 빈 공간
				case 1: std::cout << "□"; break; // 벽 (Map.txt의 1)
				case 2: std::cout << "■"; break; // 고정된 블록 (쌓인 블록)
				default: std::cout << "  "; break;
				}
			}
		}
	}
}

void TetrisLevel::LoadMapFromFile(const char* fileName)
{
	// 맵 파일 경로 생성
	char filePath[256] = {};
	sprintf_s(filePath, 256, "../Resources/Files/%s.txt", fileName);

	FILE* file = nullptr;
	fopen_s(&file, filePath, "r");

	if (file == nullptr)
	{
		std::cout << "[TetrisLevel] 맵 파일 읽기 실패: " << fileName << "\n";
		InitializeBoard(); // 기본 보드로 초기화
		return;
	}

	// 파일에서 한 줄씩 읽어서 보드에 저장
	char buffer[256];
	int row = 0;

	while (fgets(buffer, sizeof(buffer), file) && row < BOARD_HEIGHT)
	{
		int col = 0;
		int bufferIndex = 0;

		// 한 줄을 파싱해서 보드에 저장
		while (buffer[bufferIndex] != '\0' && col < BOARD_WIDTH)
		{
			char ch = buffer[bufferIndex];

			if (ch >= '0' && ch <= '9')
			{
				gameBoard[row][col] = ch - '0'; // 문자를 숫자로 변환
				col++;
			}
			else if (ch == ',')
			{
				// 콤마는 구분자이므로 무시하고 다음 문자로
			}

			bufferIndex++;
		}

		// 만약 열이 부족하면 나머지를 0으로 채움
		while (col < BOARD_WIDTH)
		{
			gameBoard[row][col] = 0;
			col++;
		}

		row++;
	}

	// 남은 행들을 0으로 채움
	while (row < BOARD_HEIGHT)
	{
		for (int col = 0; col < BOARD_WIDTH; col++)
		{
			gameBoard[row][col] = 0;
		}
		row++;
	}

	fclose(file);
}

void TetrisLevel::InitializeBoard()
{
	// 보드를 모두 빈 공간(0)으로 초기화 (맵 파일이 없을 때의 기본값)
	for (int y = 0; y < BOARD_HEIGHT; ++y)
	{
		for (int x = 0; x < BOARD_WIDTH; ++x)
		{
			gameBoard[y][x] = 0;
		}
	}

	// 기본 테트리스 보드 구조 (양 옆과 아래에 벽)
	for (int y = 0; y < BOARD_HEIGHT; ++y)
	{
		gameBoard[y][0] = 1;                    // 왼쪽 벽
		gameBoard[y][BOARD_WIDTH - 1] = 1;      // 오른쪽 벽
	}

	for (int x = 0; x < BOARD_WIDTH; ++x)
	{
		gameBoard[BOARD_HEIGHT - 1][x] = 1;     // 바닥
	}
}

void TetrisLevel::UpdateShadowBlock()
{
	if (!currentBlock || !shadowBlock)
		return;

	// 그림자 블록을 현재 블록과 같은 타입/회전으로 설정
	shadowBlock->SetBlockType(currentBlock->GetBlockType());
	shadowBlock->SetRotation(currentBlock->GetRotation());

	// 그림자 위치 계산 (TetrisLevel에서 직접 계산)
	Vector2 currentPos = currentBlock->GetGridPosition();
	Vector2 shadowPos = CalculateShadowPosition(currentPos, currentBlock->GetBlockType(), currentBlock->GetRotation());
	shadowBlock->SetGridPosition(shadowPos);
}

void TetrisLevel::ProcessCompletedLines()
{
	int cleared = ClearCompletedLines();

	if (cleared > 0)
	{
		linesCleared += cleared;

		// 점수 계산 (테트리스 표준 점수 시스템)
		int lineScore = 0;
		switch (cleared)
		{
		case 1: lineScore = 100; break;  // Single
		case 2: lineScore = 300; break;  // Double  
		case 3: lineScore = 500; break;  // Triple
		case 4: lineScore = 800; break;  // Tetris
		}

		score += lineScore * level;

		// 레벨 업 (10라인마다)
		int newLevel = (linesCleared / 10) + 1;
		if (newLevel > level)
		{
			level = newLevel;
			blockDropInterval = 0.1f >  1.0f - (level * 0.1f) ? 0.1f : 1.0f - (level * 0.1f); // 레벨이 올라갈수록 빨라짐
			std::cout << "[TetrisLevel] 레벨 업! 현재 레벨: " << level << "\n";
		}

		std::cout << "[TetrisLevel] " << cleared << "라인 클리어! 점수: +" << lineScore * level << "\n";
	}
}

Vector2 TetrisLevel::GetSpawnPosition() const
{
	// 테트리스 표준 스폰 위치 (보드 중앙 상단, 벽 고려)
	return Vector2{ BOARD_WIDTH / 2 - 2, 1 }; // Y=1 (맨 위 벽 바로 아래)
}

Vector2 TetrisLevel::CalculateShadowPosition(const Vector2& currentPos, EBlockType blockType, int rotation) const
{
	Vector2 shadowPos = currentPos;

	// 현재 위치에서 한 칸씩 아래로 내려가면서 충돌 검사
	while (true)
	{
		Vector2 testPos = { shadowPos.x, shadowPos.y + 1 };

		// 한 칸 더 아래로 이동할 수 있는지 검사
		if (!CanBlockMoveTo(testPos, blockType, rotation))
		{
			// 더 이상 내려갈 수 없으면 현재 위치가 그림자 위치
			break;
		}

		shadowPos = testPos;
	}

	return shadowPos;
}

void TetrisLevel::GenerateNextBag()
{
	// 7-bag 시스템: 7개 블록을 모두 한 번씩 나오게 한 후 섞기
	nextBlocks.clear();

	for (int i = 0; i < 7; ++i)
	{
		nextBlocks.push_back(static_cast<EBlockType>(i));
	}

	// 랜덤 셔플
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(nextBlocks.begin(), nextBlocks.end(), g);
}

EBlockType TetrisLevel::GetNextBlockType()
{
	if (nextBlocks.empty())
	{
		GenerateNextBag();
	}

	EBlockType nextType = nextBlocks.back();
	nextBlocks.pop_back();

	return nextType;
}