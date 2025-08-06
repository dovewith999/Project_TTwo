#include "TetrisMultiLevel.h"
#include "Managers/FileManager.h"
#include "Managers/SoundManager.h"
#include "Managers/NetworkManager.h"
#include "Managers/ResourceManager.h"
#include "Utils/Utils.h"

void TetrisMultiLevel::BeginPlay()
{
    super::BeginPlay();

    if (calledBeginPlay == true)
    {
        return;
    }

    calledBeginPlay = true;

    //if (this->calledBeginPlay_multiLevel == true)
    //{
    //    return;
    //}

    //this->calledBeginPlay_multiLevel = true;

    // 상대방 보드 초기화
    InitializeOpponentBoard();
}

void TetrisMultiLevel::Tick(float deltaTime)
{
    super::Tick(deltaTime);

    // 라인 처리는 클라에서 하자
    //ClearCompletedLines(); -> TetrisLevel에 있는 함수는 TetrisLevel의 GameBoard기준임
}

void TetrisMultiLevel::Render()
{
    super::Render(); // 기존 TetrisLevel의 Render 호출 (자신의 보드 + UI)

    if (!isGameStarted)
        return;

    // 상대방 보드 렌더링
    RenderOpponentBoard();

    // 멀티플레이어 전용 UI 렌더링
    //RenderMultiUI();
}

void TetrisMultiLevel::Exit()
{
    calledBeginPlay = false;
}

void TetrisMultiLevel::EndGame()
{
    super::EndGame();
    NetworkManager::GetInstance()->SendPacket(TMCP_GAME_OVER);
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

void TetrisMultiLevel::ClearOpponentCurrentBlock()
{
    // 현재 떨어지는 블록만 지우기, 고정된 블록은 유지
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            if (opponentBoard[y][x] > 1 && opponentBoard[y][x] < 100) // 현재 블록 표시값 (벽과 0, 고정된 값(기존 값 + 100)이 아니면 0으로 지움)
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
    Utils::SetConsoleCursorPosition(Vector2{ OPPONENT_START_X + 5, OPPONENT_START_Y - 1 });
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

void TetrisMultiLevel::RenderMultiUI()
{
    // NetworkManager 상태 확인
    bool isConnected = NetworkManager::GetInstance()->GetIsConnected(); // private 멤버라면 getter 함수 필요

    // 상대방 정보 표시 위치
    const int OPPONENT_UI_X = 75;
    const int OPPONENT_UI_Y = 15;

    // 연결 상태 표시
    Utils::SetConsoleCursorPosition(Vector2{ OPPONENT_UI_X, OPPONENT_UI_Y });
    std::cout << "Network Status:";

    Utils::SetConsoleCursorPosition(Vector2{ OPPONENT_UI_X, OPPONENT_UI_Y + 1 });
    if (isConnected)
    {
        Utils::SetConsoleTextColor(Color::LightGreen);
        std::cout << "CONNECTED";
    }
    else
    {
        Utils::SetConsoleTextColor(Color::Red);
        std::cout << "DISCONNECTED";
    }
    Utils::SetConsoleTextColor(Color::White);

    // 게임 상태 표시
    Utils::SetConsoleCursorPosition(Vector2{ OPPONENT_UI_X, OPPONENT_UI_Y + 3 });
    if (!isConnected)
    {
        Utils::SetConsoleTextColor(Color::Yellow);
        std::cout << "Waiting for connection...";
        Utils::SetConsoleTextColor(Color::White);
    }
    else
    {
        std::cout << "Playing with opponent";
    }

    // 멀티플레이어 조작법
    Utils::SetConsoleCursorPosition(Vector2{ OPPONENT_UI_X, OPPONENT_UI_Y + 5 });
    std::cout << "Multi Controls:";
    Utils::SetConsoleCursorPosition(Vector2{ OPPONENT_UI_X, OPPONENT_UI_Y + 6 });
    std::cout << "ESC: Disconnect";
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
