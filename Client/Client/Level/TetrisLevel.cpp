#include "TetrisLevel.h"
#include "Utils/Define.h"
#include "Utils/Utils.h"
#include "Core/Input.h"
#include "Managers/SoundManager.h"
#include "Managers/FileManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/LevelManager.h"
#include "Managers/NetworkManager.h"
#include "Controller/TetrisController.h"
#include "TetrisMultiLevel.h"

#include <iostream>
#include <algorithm>
#include <random>

TetrisLevel::TetrisLevel()
{

}

TetrisLevel::~TetrisLevel()
{
	SafeDelete(currentBlock);
	SafeDelete(shadowBlock);
	//currentBlock = nullptr;
	//shadowBlock = nullptr;
	SafeDelete(controller);
}

void TetrisLevel::BeginPlay()
{
	super::BeginPlay();

 	if (calledBeginPlay == true)
	{
		return;
	}

	InitializeBoard();
	GenerateNextBag();

	if (LevelManager::GetInstance()->GetCurrentLevel()->As<TetrisMultiLevel>() == nullptr)
	{
		calledBeginPlay = true;
	}

	system("cls");

	// 리소스 로드
	ResourceManager::GetInstance()->Initialize();

	// 맵 파일 읽어서 게임 보드 초기화
	LoadMapFromFile("Map");

	// 블럭을 컨트롤 할 컨트롤러 생성
	controller = new TetrisController(this);

	elapsedTime = remainingTime;

	// 게임 시작
	StartGame();
}

void TetrisLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!isGameStarted || isGamePaused || isGameOver)
		return;

	// 입력 처리
	controller->ControllBlock();

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
				if (LevelManager::GetInstance()->GetCurrentLevel()->As<TetrisMultiLevel>() != nullptr)
				{
					NetworkManager::GetInstance()->SendInput(currentBlock, VK_DOWN);
				}
			}
			else
			{
				// 블록이 더 이상 떨어질 수 없음 - 고정하고 새 블록 생성
				PlaceBlockOnBoard(currentBlock);
				if (LevelManager::GetInstance()->GetCurrentLevel()->As<TetrisMultiLevel>() != nullptr)
				{
					NetworkManager::GetInstance()->SendInput(currentBlock, VK_DOWN, true);
				}
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

	// 보드 출력
	RenderBoard();

	//  TODO : Render에서 매 프레임마다 호출할 필요가 있나. 특히 조작법 같은 거
	RenderUI();
}

void TetrisLevel::Exit()
{
	calledBeginPlay = false;
}

void TetrisLevel::EndPlay()
{
	EndGame();
	isGameOver = false;
}

void TetrisLevel::StartGame()
{
	// BGM 시작
	SoundManager::GetInstance()->PlaySoundW(L"BGM_INGAME.wav", Define::ESoundChannelID::BGM, 3.f);

	isGameStarted = true;
	isGamePaused = false;
	isGameOver = false;
	isWaitingForGameResult = false;

	score = 0;
	linesCleared = 0;
	level = 1;

	blockDropTimer = 0.0f;
	blockDropInterval = 1.0f;

	// 시간 관리 초기화
	gameStartTime = time(NULL);
	remainingTime = 60; // 1분
	isMultiplayLevel = (LevelManager::GetInstance()->GetCurrentLevel()->As<TetrisMultiLevel>() != nullptr);

	memset(nextBlockUI, 0, sizeof(nextBlockUI));

	InitializeBoard();

	nextBlockType = GetNextBlockType();
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
	if (!isMultiplayLevel)
	{
		isGameStarted = false;
		isGameOver = true;
		Exit();

		SafeDelete(currentBlock);
		SafeDelete(shadowBlock);
		system("cls");

		std::cout << "[TetrisLevel] 게임 종료! 최종 점수: " << score << "\n";

		Sleep(3000);
		system("cls");
		LevelManager::GetInstance()->ChangeLevel(Define::ELevel::TITLE);
	}
}

void TetrisLevel::SpawnNewBlock()
{
	// 이전 블록 정리
	SafeDelete(currentBlock);
	SafeDelete(shadowBlock);

	// 새 블록 생성
	newBlockType = nextBlockType;
	Vector2 spawnPos = GetSpawnPosition();

	// 그 다음에 생성될 블록 타입 저장
	nextBlockType = GetNextBlockType();

	const BlockShapeData* data = ResourceManager::GetInstance()->GetBlockShape(static_cast<int>(nextBlockType), 0);

	Color color = Color::White;

	switch (newBlockType)
	{
	case EBlockType::I: color = Color::LightBlue; break;
	case EBlockType::O: color = Color::Yellow; break;
	case EBlockType::T: color = Color::Purple; break;
	case EBlockType::S: color = Color::Red; break;
	case EBlockType::Z: color = Color::LightGreen; break;
	case EBlockType::J: color = Color::Blue; break;
	case EBlockType::L: color = Color::Orange; break;
	case EBlockType::None: break;
	default: break;
	}

	currentBlock = new TetrisBlock(newBlockType, spawnPos, EBlockState::Falling, color);
	shadowBlock = new TetrisBlock(newBlockType, spawnPos, EBlockState::Shadow);

	controller->SetCurrentBlock(currentBlock);

	//// 블록 시작
	currentBlock->BeginPlay();
	shadowBlock->BeginPlay();

	// 게임 오버 체크 (스폰 위치에 이미 블록이 있으면)
	if (!CanBlockMoveTo(spawnPos, newBlockType, 0))
	{
		isGameOver = true;
		std::cout << "[TetrisLevel] 게임 오버! 새 블록을 생성할 공간이 없습니다.\n";
	}

#pragma region Next Block UI
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (data->GetPixel(j, i) == 0)
			{
				nextBlockUI[j][i] = "  ";
			}

			else
			{
				nextBlockUI[j][i] = "■";
			}
		}
	}

	switch (nextBlockType)
	{
	case EBlockType::I: color = Color::LightBlue; break;
	case EBlockType::O: color = Color::Yellow; break;
	case EBlockType::T: color = Color::Purple; break;
	case EBlockType::S: color = Color::Red; break;
	case EBlockType::Z: color = Color::LightGreen; break;
	case EBlockType::J: color = Color::Blue; break;
	case EBlockType::L: color = Color::Orange; break;
	case EBlockType::None: break;
	default: break;
	}

	Vector2 nextRenderPosition(BOARD_START_X + 26, 5);

	// 다음에 나올 블럭을 출력
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			Utils::SetConsoleTextColor(color);
			Utils::SetConsoleCursorPosition(Vector2{ nextRenderPosition.x + j * 2, nextRenderPosition.y + i });
			std::cout << nextBlockUI[j][i];
		}
	}
#pragma endregion
}

bool TetrisLevel::IsGameOver() const
{
	if (isGameOver || isWaitingForGameResult)
	{
		return true;
	}

	// 멀티플레이어에서는 블럭이 한계선을 넘으면 게임 오버
	if (isMultiplayLevel)
	{
		// 게임 보드의 상단 3줄 중 하나라도 블럭이 있으면 게임 오버
		for (int y = 1; y <= 3; ++y) // 맨 위 벽 제외하고 3줄 체크
		{
			for (int x = 1; x < BOARD_WIDTH - 1; ++x) // 양쪽 벽 제외
			{
				if (gameBoard[y][x] != 0)
				{
					// 서버에게 게임 오버 알림
					if (NetworkManager::GetInstance()->GetIsConnected())
					{
						NetworkManager::GetInstance()->SendGameOver();
					}
					return true;
				}
			}
		}
	}
	else
	{
		if (elapsedTime == 0)
		{
			return true;
		}
	}

	return false;
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
	{
		return;
	}

	Vector2 pos = block->GetGridPosition();
	int typeIndex = static_cast<int>(block->GetBlockType());
	const BlockShapeData* shapeData = ResourceManager::GetInstance()->GetBlockShape(typeIndex, block->GetRotation());

	if (!shapeData)
		return;

	// 보드에 블록 고정
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			if (shapeData->GetPixel(x, y))
			{
				int boardX = pos.x + x;
				int boardY = pos.y + y;

				if (boardX >= 1 && boardX < BOARD_WIDTH - 1 && boardY >= 0 && boardY < BOARD_HEIGHT - 1)
				{
					gameBoard[boardY][boardX] = block->GetBoardMarker(); // 색을 차이내기 위한 Marker값으로 저장
				}
			}
		}
	}
}

int TetrisLevel::ClearCompletedLines()
{
	int clearedLines = 0;

	// 아래에서 위로 라인 체크 (벽 제외하고 1~11열만)
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
	// 게임 보드 출력 (기존 테트리스와 동일한 위치 - gotoxy(BOARD_START_X, y + BOARD_START_Y))
	for (int y = 0; y < BOARD_HEIGHT; ++y)
	{
		Utils::SetConsoleCursorPosition(Vector2{ BOARD_START_X, y + BOARD_START_Y });

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

								if (worldX == x && worldY == y && worldY >= 0)
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
				Utils::SetConsoleTextColor(currentBlock->GetColor());
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
				case 1:
					Utils::SetConsoleTextColor(Color::White);
					std::cout << "□"; break; // 벽 (Map.txt의 1)
				case 2:  // I
					Utils::SetConsoleTextColor(Color::LightBlue);
					std::cout << "■";
					break; // 고정된 블록 (쌓인 블록)
				case 3: // O
					Utils::SetConsoleTextColor(Color::Yellow);
					std::cout << "■";
					break; // 고정된 블록 (쌓인 블록)
				case 4: // T
					Utils::SetConsoleTextColor(Color::Purple);
					std::cout << "■";
					break; // 고정된 블록 (쌓인 블록)
				case 5: // S
					Utils::SetConsoleTextColor(Color::Red);
					std::cout << "■";
					break; // 고정된 블록 (쌓인 블록)
				case 6: // Z
					Utils::SetConsoleTextColor(Color::LightGreen);
					std::cout << "■";
					break; // 고정된 블록 (쌓인 블록)
				case 7: // J
					Utils::SetConsoleTextColor(Color::Blue);
					std::cout << "■";
					break; // 고정된 블록 (쌓인 블록)
				case 8: // L
					Utils::SetConsoleTextColor(Color::Orange);
					std::cout << "■";
					break; // 고정된 블록 (쌓인 블록)
				case 9: // 공격 라인
					Utils::SetConsoleTextColor(Color::DarkGray);
					std::cout << "▨";
					break;
				default:
					Utils::SetConsoleTextColor(Color::White);
					std::cout << "  ";
					break;
				}
				Utils::SetConsoleTextColor(Color::White);
			}
		}
	}
}

void TetrisLevel::RenderUI()
{
	// UI 정보 출력 TODO : 한번만 나오면 되는데 얘네는
	Utils::SetConsoleTextColor(Color::Orange);
	Utils::SetConsoleCursorPosition(Vector2{ BOARD_START_X - 13, 3 }); // TODO : 엄청난 하드코딩이다.ㄱ-
	std::cout << "Save Block";

	Utils::SetConsoleTextColor(Color::Orange);
	Utils::SetConsoleCursorPosition(Vector2{ BOARD_START_X + 26, 3 }); // TODO : 엄청난 하드코딩이다.ㄱ-
	std::cout << "Next Block";

	Utils::SetConsoleTextColor(Color::White);
	const int uiStartX = BOARD_START_X + 26;
	// 시간 표시
	Utils::SetConsoleCursorPosition(Vector2{ uiStartX, 15 });
	int minutes = 0;
	int seconds = 0;

	if (isMultiplayLevel)
	{
		// 멀티플레이어 모드 - 서버에서 받은 남은 시간 표시
		minutes = (int)remainingTime / 60;
		seconds = (int)remainingTime % 60;

		// 시간이 10초 이하면 빨간색으로 표시
		if (remainingTime <= 10)
			Utils::SetConsoleTextColor(Color::Red);
		else if (remainingTime <= 30)
			Utils::SetConsoleTextColor(Color::Yellow);
		else
			Utils::SetConsoleTextColor(Color::White);
	}

	else
	{
		// 싱글플레이어 모드 - 경과 시간 표시
		elapsedTime = remainingTime - (time(NULL) - gameStartTime);
		minutes = (int)elapsedTime / 60;
		seconds = (int)elapsedTime % 60;

		// 시간이 10초 이하면 빨간색으로 표시
		if (elapsedTime <= 10)
			Utils::SetConsoleTextColor(Color::Red);
		else if (elapsedTime <= 30)
			Utils::SetConsoleTextColor(Color::Yellow);
		else
			Utils::SetConsoleTextColor(Color::White);
	}

	std::cout << "Time  :  " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
	Utils::SetConsoleTextColor(Color::White);

	Utils::SetConsoleCursorPosition(Vector2{ uiStartX, 16 });
	std::cout << "Score :  " << score;

	Utils::SetConsoleCursorPosition(Vector2{ uiStartX, 17 });
	std::cout << "Level :  " << level;

	Utils::SetConsoleCursorPosition(Vector2{ uiStartX, 18 });
	std::cout << "Lines :  " << linesCleared;

	// 조작법 안내
	const int settingStartX = BOARD_START_X - 1;
	Utils::SetConsoleCursorPosition(Vector2{ settingStartX, 25 });
	std::cout << "조작법:";
	Utils::SetConsoleCursorPosition(Vector2{ settingStartX, 26 });
	std::cout << "←→: 이동  ↑: 회전";
	Utils::SetConsoleCursorPosition(Vector2{ settingStartX, 27 });
	std::cout << "↓: 소프트드롭  SPACE: 하드드롭";
	Utils::SetConsoleCursorPosition(Vector2{ settingStartX, 28 });
	std::cout << "TAB: 블럭 Save & Switching";
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
		case 1: 
			lineScore = 100;
			SoundManager::GetInstance()->PlaySoundW(L"SINGLE.wav", Define::ESoundChannelID::SUBEFFECT, 10.f);
			break;  // 1줄 클리어
		case 2:
			lineScore = 300;
			SoundManager::GetInstance()->PlaySoundW(L"DOUBLE.wav", Define::ESoundChannelID::SUBEFFECT, 10.f);
			break;  // 2줄 클리어  
		case 3: 
			lineScore = 500; 
			SoundManager::GetInstance()->PlaySoundW(L"TRIPLE.wav", Define::ESoundChannelID::SUBEFFECT, 10.f);
			break;  // 3줄 클리어
		case 4: 
			lineScore = 800; 
			SoundManager::GetInstance()->PlaySoundW(L"TETRIS.wav", Define::ESoundChannelID::SUBEFFECT, 10.f);
			break;  // 4줄 클리어(Tetris)
		}

		score += lineScore * level;

		// 레벨 업 (10라인마다)
		int newLevel = (linesCleared / 10) + 1;
		if (newLevel > level)
		{
			level = newLevel;
			blockDropInterval = 0.1f > 1.0f - (level * 0.1f) ? 0.1f : 1.0f - (level * 0.1f); // 레벨이 올라갈수록 빨라짐
		}
	}
}
// TODO : 현재 컨트롤 중인 블럭을 saveBlock으로 두고 다음 블럭 가져오기, 혹시 saveBlock이 None이 아니라면 save블럭을 컨트롤하기
void TetrisLevel::SwitchBlock()
{
	Color color = Color::White;

	if (saveBlockType != EBlockType::None)
	{
		switch (saveBlockType)
		{
		case EBlockType::I: color = Color::LightBlue; break;
		case EBlockType::O: color = Color::Yellow; break;
		case EBlockType::T: color = Color::Purple; break;
		case EBlockType::S: color = Color::Red; break;
		case EBlockType::Z: color = Color::LightGreen; break;
		case EBlockType::J: color = Color::Blue; break;
		case EBlockType::L: color = Color::Orange; break;
		case EBlockType::None: break;
		default: break;
		}

		EBlockType tempBlockType = currentBlock->GetBlockType();

		SafeDelete(currentBlock);
		SafeDelete(shadowBlock);

		currentBlock = new TetrisBlock(saveBlockType, GetSpawnPosition(), EBlockState::Falling, color);
		shadowBlock = new TetrisBlock(saveBlockType, GetSpawnPosition(), EBlockState::Shadow);

		controller->SetCurrentBlock(currentBlock);
		
		saveBlockType = tempBlockType;
	}

	else if (saveBlockType == EBlockType::None)
	{
		saveBlockType = currentBlock->GetBlockType();
		SpawnNewBlock();
	}


#pragma region save Block UI

	// 바뀐 SaveBlockType으로 색 재지정
	switch (saveBlockType)
	{
	case EBlockType::I: color = Color::LightBlue; break;
	case EBlockType::O: color = Color::Yellow; break;
	case EBlockType::T: color = Color::Purple; break;
	case EBlockType::S: color = Color::Red; break;
	case EBlockType::Z: color = Color::LightGreen; break;
	case EBlockType::J: color = Color::Blue; break;
	case EBlockType::L: color = Color::Orange; break;
	case EBlockType::None: break;
	default: break;
	}

	const BlockShapeData* data = ResourceManager::GetInstance()->GetBlockShape(static_cast<int>(saveBlockType), 0);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (data->GetPixel(j, i) == 0)
			{
				saveBlockUI[j][i] = "  ";
			}

			else
			{
				saveBlockUI[j][i] = "■";
			}
		}
	}

	Vector2 saveRenderPosition(BOARD_START_X - 13, 5);

	// 다음에 나올 블럭을 출력
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			Utils::SetConsoleTextColor(color);
			Utils::SetConsoleCursorPosition(Vector2{ saveRenderPosition.x + j * 2, saveRenderPosition.y + i });
			std::cout << saveBlockUI[j][i];
		}
	}
#pragma endregion
}

Vector2 TetrisLevel::GetSpawnPosition() const
{
	// 테트리스 표준 스폰 위치 (보드 중앙 상단, 벽 고려)
	return Vector2{ BOARD_WIDTH / 2 - 2, 0 }; // Y=1 (맨 위 벽 바로 아래)
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

	//BlockType이 2~9부터기 때문에 이런 조건으로 반복
	for (int i = 2; i < 9; ++i)
	{
		nextBlocks.emplace_back(static_cast<EBlockType>(i));
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