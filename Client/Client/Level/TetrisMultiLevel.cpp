#include "TetrisMultiLevel.h"
#include "Managers/FileManager.h"
#include "Managers/SoundManager.h"
#include "Managers/LevelManager.h"
#include "Managers/NetworkManager.h"
#include "Managers/ResourceManager.h"
#include "Utils/Utils.h"

#include <random>

void TetrisMultiLevel::BeginPlay()
{
    // 기다리고 있는 상태면 아무것도 안하고 리턴
	if (NetworkManager::GetInstance()->GetIsWaitingForOpponent())
	{
        return;
    }

	super::BeginPlay();

	if (calledBeginPlay == true)
	{
		return;
	}

	calledBeginPlay = true;

	// 상대방 보드 초기화
	InitializeOpponentBoard();

	// 네트워크 연결 시작
	if (!NetworkManager::GetInstance()->GetIsConnected())
	{
		NetworkManager::GetInstance()->AcceptServer();
	}

    isGameTimeLimited = true;
    remainingTime = 60; // 1분
}

void TetrisMultiLevel::Tick(float deltaTime)
{
	if (NetworkManager::GetInstance()->GetIsWaitingForOpponent())
	{
		return; // 대기 중이므로 게임 진행 안함
	}

	// 게임이 시작된 후에만 부모 클래스 Tick 호출
	if (NetworkManager::GetInstance()->GetIsGameStarted())
	{
		super::Tick(deltaTime);
	}

}

void TetrisMultiLevel::Render()
{
	// 상대방을 기다리는 중이면 대기 화면 표시
	if (NetworkManager::GetInstance()->GetIsWaitingForOpponent())
	{
		RenderWaitingScreen();
		return;
	}

	// 게임이 시작되었을 때만 일반 렌더링
	if (NetworkManager::GetInstance()->GetIsGameStarted())
	{
        super::Render();

		// 상대방 보드 렌더링
		RenderOpponentBoard();
	}
}

void TetrisMultiLevel::Exit()
{
    calledBeginPlay = false;
}

bool TetrisMultiLevel::IsGameOver() const
{
    // 부모 클래스의 게임오버 조건 체크
    if (TetrisLevel::IsGameOver())
    {
        return true;
    }

    // 시간이 다 되었을 때
    if (remainingTime <= 0)
    {
        return true;
    }

    return false;
}

void TetrisMultiLevel::EndGame()
{
    super::EndGame();
    NetworkManager::GetInstance()->SendPacket(TMCP_GAME_OVER);
}

int TetrisMultiLevel::ClearCompletedLines()
{
   // 부모 클래스의 라인 클리어 로직 호출
    int clearedLines = TetrisLevel::ClearCompletedLines();
    
    // 멀티플레이어에서만 공격 로직 추가
    if (clearedLines >= 2 && NetworkManager::GetInstance()->GetIsConnected())
    {
        // 테트리스 표준 공격 라인 수 계산
        int attackLines = 0;
        switch (clearedLines)
        {
        case 2: attackLines = 1; break;  // Double -> 1줄 공격
        case 3: attackLines = 2; break;  // Triple -> 2줄 공격  
        case 4: attackLines = 4; break;  // Tetris -> 4줄 공격
        default: attackLines = 0; break; // Single은 공격 없음
        }
        
        if (attackLines > 0)
        {
            // 구멍 패턴을 미리 생성
            TMCPAttackData attackData;
            attackData.attackLines = attackLines;
            
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, BOARD_WIDTH - 2);
            
            for (int i = 0; i < attackLines; ++i)
            {
                attackData.holePositions[i] = dis(gen);
            }
            
            // NetworkManager에게 공격 패킷 전송 (구멍 위치 포함)
            NetworkManager::GetInstance()->SendAttackLines(attackData);
        }
    }

    if (clearedLines > 0)
    {
		// Multi Level에서만 점수 보내면 됨.
		NetworkManager::GetInstance()->SendScore(score);
    }

    return clearedLines;
}

void TetrisMultiLevel::ReceiveAttackFromOpponent(const TMCPAttackData& attackData)
{
	if (attackData.attackLines > 0)
	{
		AddAttackLinesToBottom(attackData);
	}
}

void TetrisMultiLevel::AddAttackLinesToOpponentBoard(const TMCPAttackData& attackData)
{
	int lines = attackData.attackLines;

	// 상대방 보드를 위로 밀어올리기 (시각적 표현용)
	for (int y = 1; y < BOARD_HEIGHT - lines - 1; ++y)  // 벽 제외
	{
		for (int x = 1; x < BOARD_WIDTH - 1; ++x)  // 양쪽 벽 제외
		{
			opponentBoard[y][x] = opponentBoard[y + lines][x];
		}
	}

	// 상대방 보드 바닥에 공격 라인 추가 (동일한 구멍 위치 사용)
	for (int i = 0; i < lines; ++i)
	{
		int targetY = BOARD_HEIGHT - 2 - i;  // 바닥에서부터 위로
		int holeX = attackData.holePositions[i];  // 동일한 구멍 위치 사용

		// 해당 라인을 공격 블록으로 채우기 (구멍 제외)
		for (int x = 1; x < BOARD_WIDTH - 1; ++x)
		{
			if (x == holeX)
			{
				opponentBoard[targetY][x] = 0;  // 구멍
			}
			else
			{
				opponentBoard[targetY][x] = 9;  // 공격 라인 마커
			}
		}
	}

    SoundManager::GetInstance()->PlaySoundW(L"ATTACK.wav", Define::ESoundChannelID::ATTACKEFFECT, 3.f);
}

void TetrisMultiLevel::SetRemainingTime(time_t time)
{
    remainingTime = time;
}
void TetrisMultiLevel::InitializeOpponentBoard()
{
    // 상대방 보드를 기본 테트리스 보드 구조로 초기화
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            opponentBoard[y][x] = 0;
        }
    }

    // 벽 구조 초기화
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        opponentBoard[y][0] = 1;                    // 왼쪽 벽
        opponentBoard[y][BOARD_WIDTH - 1] = 1;      // 오른쪽 벽
    }

    for (int x = 0; x < BOARD_WIDTH; ++x)
    {
        opponentBoard[BOARD_HEIGHT - 1][x] = 1;     // 바닥
    }
}

void TetrisMultiLevel::UpdateOpponentBoard(const TMCPBlockData& blockData)
{
    // 이전 상대방 블록 위치 지우기 (고정된 블록은 유지)
    ClearOpponentCurrentBlock();

    // 새로운 블록 위치에 표시
    // blockData의 정보를 사용해서 상대방 보드에 블록 표시
    int blockType = blockData.blockType + 1; // 1~7 값을 2~8 값으로 보간
    int posX = blockData.posX;
    int posY = blockData.posY;
    int rotation = blockData.rotation;

    int marker = blockType; // 보드에 저장되는 값

    // ResourceManager에서 블록 모양 데이터 가져오기
    const BlockShapeData* shapeData = ResourceManager::GetInstance()->GetBlockShape(blockType, rotation);

    if (blockData.action == 2) // 고정되는 액션이라면
    {
        marker += 100; // 보드에 고정되는 값으로 변경
    }

    if (shapeData)
    {
        for (int y = 0; y < 4; ++y)
        {
            for (int x = 0; x < 4; ++x)
            {
                if (shapeData->GetPixel(x, y))
                {
                    int boardX = posX + x;
                    int boardY = posY + y;

                    if (boardX >= 0 && boardX < BOARD_WIDTH && boardY >= 0 && boardY < BOARD_HEIGHT)
                    {
                        // 현재 떨어지는 블록으로 표시 (임시값 99 사용)
                        opponentBoard[boardY][boardX] = marker;
                    }
                }
            }
        }
    }

    if (blockData.action == 2)
    {
        ClearOpponentCompletedLines(); // 라인처리가 가능한지 확인
    }
}

void TetrisMultiLevel::AddAttackLinesToBottom(const TMCPAttackData& attackData)
{   
	int lines = attackData.attackLines;

	// 현재 보드를 위로 밀어올리기
	for (int y = 1; y < BOARD_HEIGHT - lines - 1; ++y)  // 벽 제외
	{
		for (int x = 1; x < BOARD_WIDTH - 1; ++x)  // 양쪽 벽 제외
		{
			gameBoard[y][x] = gameBoard[y + lines][x];
		}
	}

	// 바닥 근처에 공격 라인 추가 (전달받은 구멍 위치 사용)
	for (int i = 0; i < lines; ++i)
	{
		int targetY = BOARD_HEIGHT - 2 - i;  // 바닥에서부터 위로
		int holeX = attackData.holePositions[i];  // 전달받은 구멍 위치 사용

		// 해당 라인을 공격 블록으로 채우기 (구멍 제외)
		for (int x = 1; x < BOARD_WIDTH - 1; ++x)
		{
			if (x == holeX)
			{
				gameBoard[targetY][x] = 0;  // 구멍
			}
			else
			{
				gameBoard[targetY][x] = 9;  // 공격 라인 마커
			}
		}
	}

	// 현재 블록이 새로 추가된 라인과 겹치는지 확인하고 게임오버 처리
	if (currentBlock && !CanBlockMoveTo(currentBlock->GetGridPosition(),
		currentBlock->GetBlockType(), currentBlock->GetRotation()))
	{
		isGameOver = true;
	}
}

void TetrisMultiLevel::ClearOpponentCurrentBlock()
{
    // 현재 떨어지는 블록만 지우기, 고정된 블록은 유지
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (opponentBoard[y][x] > 1 && opponentBoard[y][x] < 100 && opponentBoard[y][x] != 9) // 현재 블록 표시값 (벽과 0, 고정된 값(기존 값 + 100), 공격당한 블럭(9)가 아니면 0으로 지움)
            {
                opponentBoard[y][x] = 0; // 빈 공간으로 변경
            }
        }
    }
}

void TetrisMultiLevel::RenderOpponentBoard()
{
    // 상대방 보드 시작 위치 (기존 보드 오른쪽)
    const int OPPONENT_START_X = BOARD_START_X + 55;
    const int OPPONENT_START_Y = BOARD_START_Y;

    // 상대방 보드 제목
    Utils::SetConsoleCursorPosition(Vector2{ OPPONENT_START_X + 7, OPPONENT_START_Y - 1 });
    Utils::SetConsoleTextColor(Color::Yellow);
    std::cout << "OPPONENT";
    Utils::SetConsoleTextColor(Color::White);

    // 상대방 보드 렌더링
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        Utils::SetConsoleCursorPosition(Vector2{ OPPONENT_START_X, y + OPPONENT_START_Y });

        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            int cellValue = opponentBoard[y][x];

            switch (cellValue)
            {
            case 0:
                std::cout << "  ";
                break; // 빈 공간
            case 1:
            case 101:
                Utils::SetConsoleTextColor(Color::White);
                std::cout << "□";
                break; // 벽
            case 2:  // I 블록 (고정)
            case 102:
                Utils::SetConsoleTextColor(Color::LightBlue);
                std::cout << "■";
                break;
            case 3: // O 블록 (고정)
            case 103:
                Utils::SetConsoleTextColor(Color::Yellow);
                std::cout << "■";
                break;
            case 4: // T 블록 (고정)
            case 104:
                Utils::SetConsoleTextColor(Color::Purple);
                std::cout << "■";
                break;
            case 5: // S 블록 (고정)
            case 105:
                Utils::SetConsoleTextColor(Color::Red);
                std::cout << "■";
                break;
            case 6: // Z 블록 (고정)
            case 106:
                Utils::SetConsoleTextColor(Color::LightGreen);
                std::cout << "■";
                break;
            case 7: // J 블록 (고정)
            case 107:
                Utils::SetConsoleTextColor(Color::Blue);
                std::cout << "■";
                break;
            case 8: // L 블록 (고정)
            case 108:
                Utils::SetConsoleTextColor(Color::Orange);
                std::cout << "■";
                break;
			case 9: // 공격 라인
				Utils::SetConsoleTextColor(Color::DarkGray);
				std::cout << "▨";
				break;
            case 99: // 현재 떨어지는 블록 (상대방)
                Utils::SetConsoleTextColor(Color::DarkGray);
                std::cout << "▣"; // 상대방 블록은 다른 문자로 구분
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

// TetrisMultiLevel.cpp에 추가
void TetrisMultiLevel::RenderWaitingScreen()
{
	// 화면 지우기
	//system("cls");

	// 대기 메시지 중앙에 표시
	Utils::SetConsoleCursorPosition(Vector2{ 30, 10 });
	Utils::SetConsoleTextColor(Color::Yellow);
	std::cout << "=================================";

	Utils::SetConsoleCursorPosition(Vector2{ 30, 11 });
	std::cout << "  상대방을 기다리는 중...";

	Utils::SetConsoleCursorPosition(Vector2{ 30, 12 });
	std::cout << "  Waiting for opponent...";

	Utils::SetConsoleCursorPosition(Vector2{ 30, 13 });
	std::cout << "=================================";

	Utils::SetConsoleCursorPosition(Vector2{ 30, 15 });
	Utils::SetConsoleTextColor(Color::Red);
	std::cout << "ESC: 취소하고 메인으로 돌아가기";

	Utils::SetConsoleTextColor(Color::White);

	// ESC 키 처리 (대기 중에도 나가기 가능)
	if (Input::GetInstance().GetKeyDown(VK_ESCAPE))
	{
		NetworkManager::GetInstance()->Disconnect();
		LevelManager::GetInstance()->ChangeLevel(Define::ELevel::TITLE);
	}
}

void TetrisMultiLevel::ClearOpponentCompletedLines()
{
    // 아래에서 위로 라인 체크 (벽 제외하고 1~11열만)
    for (int y = BOARD_HEIGHT - 2; y >= 1; --y) // 바닥(20행)과 맨 위(0행) 제외
    {
        bool isLineFull = true;

        // 해당 라인이 가득 찼는지 확인 (벽 제외)
        for (int x = 1; x < BOARD_WIDTH - 1; ++x) // 양옆 벽 제외
        {
            if (opponentBoard[y][x] == 0)
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
                    opponentBoard[moveY][x] = opponentBoard[moveY - 1][x];
                }
            }

            // 맨 위 라인은 빈 공간으로 (벽 제외)
            for (int x = 1; x < BOARD_WIDTH - 1; ++x)
            {
                opponentBoard[1][x] = 0;
            }
            y++;
        }
    }
}
