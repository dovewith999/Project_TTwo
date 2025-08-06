#include "NetworkManager.h"
#include "Game/Game.h"
#include "Actor/TetrisBlock.h"
#include "Utils/Utils.h"
#include "Managers/LevelManager.h"
#include "Level/TetrisMultiLevel.h"

#include <conio.h>

UINT WINAPI NetworkManager::ReceiveThread(LPVOID param)
{
	u_char cmd;
	char payload[1024];

	std::cout << "[수신 스레드] 시작 - 서버 패킷 대기 중...\n";

	while (NetworkManager::GetInstance()->isConnected)
	{
		int payloadSize = recvTMCP((unsigned int)NetworkManager::GetInstance()->clientSocket, &cmd, payload, sizeof(payload));

		if (payloadSize < 0) {
			std::cout << "[수신 스레드] 서버 연결 끊김\n";
			NetworkManager::GetInstance()->isConnected = false;
			break;
		}

		// 받은 패킷 처리
		switch (cmd)
		{
		case TMCP_CONNECT_RES:
			std::cout << "서버 연결 성공\n";
			break;

		case TMCP_MATCH_WAIT:
			std::cout << "상대방을 기다리는 중...\n";
			break;

		case TMCP_MATCH_FOUND:
			std::cout << "상대방 발견\n";
			break;

		case TMCP_GAME_START:
			//std::cout << "게임 시작! 방향키로 테스트하세요!\n";
			//std::cout << "   ↑: 위    ↓: 아래\n";
			//std::cout << "   ←: 왼쪽  →: 오른쪽\n";
			//std::cout << "   ESC: 종료\n\n";
			NetworkManager::GetInstance()->isGameStarted = true;
			break;

		case TMCP_BLOCK_MOVE:
			if (payloadSize >= sizeof(TMCPBlockData))
			{
				TMCPBlockData* blockData = (TMCPBlockData*)payload;
				//Utils::SetConsoleCursorPosition(Vector2{ 70, 40 });
				//printf("타입=%d, 회전=%d\n",
				//	blockData->blockType, blockData->rotation);

				if (LevelManager::GetInstance()->GetCurrentLevel()->As<TetrisMultiLevel>() != nullptr)
				{
					dynamic_cast<TetrisMultiLevel*>(LevelManager::GetInstance()->GetCurrentLevel())->UpdateOpponentBoard(*blockData);
				}
			}
			break;

		case TMCP_GAME_OVER:
			std::cout << "게임 종료\n";
			NetworkManager::GetInstance()->isGameStarted = false;
			break;

		case TMCP_DISCONNECT_REQ:
			std::cout << "서버에서 연결 종료 요청\n";
			NetworkManager::GetInstance()->isConnected = false;
			break;

		default:
			break;
		}
	}

	std::cout << "[수신 스레드] 종료\n";
	return 0;
}

NetworkManager::~NetworkManager()
{
	Disconnect();
}

void NetworkManager::Disconnect()
{
	isConnected = false;

	if (receiveHandle) 
	{
		WaitForSingleObject(receiveHandle, 2000);  // 2초 대기
		CloseHandle(receiveHandle);
	}

	if (clientSocket != INVALID_SOCKET) 
	{
		closesocket(clientSocket);
	}

	WSACleanup();
}

void NetworkManager::SendInput(TetrisBlock* block, int input)
{	
	TMCPBlockData blockData;

	// 방향에 따라 블록 데이터 설정
	blockData.blockType = static_cast<int>(block->GetBlockType()) - 1;  // 2~8인 값을 1~7로 보정
	blockData.rotation = block->GetRotation();
	blockData.direction = input;
	blockData.posX = block->GetGridPosition().x;
	blockData.posY = block->GetGridPosition().y;

	switch (input) {
	case VK_LEFT: // 왼쪽
		blockData.action = 0;     // 이동
		break;
	case VK_RIGHT: // 오른쪽  
		blockData.action = 0;     // 이동
		break;
	case VK_DOWN: // 아래
		blockData.action = 0;     // 이동
		break;
	case VK_UP: // 위 (회전으로 처리)
		blockData.action = 1;     // 이동
		break;
	case VK_SPACE:
		blockData.action = 3;
		break;
	default:
		break;
	}

	blockData.timestamp = (u_int)time(NULL);

	// 서버로 패킷 전송
	int result = sendTMCP((unsigned int)clientSocket, TMCP_BLOCK_MOVE, &blockData, sizeof(blockData));
	if (result < 0) {
		printf("❌ 패킷 전송 실패!\n");
	}
}

void NetworkManager::SendDirectionKey(int direction)
{
	TMCPBlockData blockData;

	// 방향에 따라 블록 데이터 설정
	blockData.blockType = 3;  // T-블록으로 고정
	blockData.rotation = 0;
	blockData.action = 0;     // 이동
	blockData.direction = direction;

	// 방향에 따른 좌표 설정 (테스트용)
	static u_short testX = 5, testY = 10;

	switch (direction) {
	case 1: // 왼쪽
		if (testX > 0) testX--;
		printf("왼쪽 이동 전송 (X=%d)\n", testX);
		break;
	case 2: // 오른쪽  
		if (testX < 9) testX++;
		printf("오른쪽 이동 전송 (X=%d)\n", testX);
		break;
	case 3: // 아래
		if (testY < 19) testY++;
		printf("아래 이동 전송 (Y=%d)\n", testY);
		break;
	case 0: // 위 (회전으로 처리)		
		blockData.rotation = (blockData.rotation + 1) % 4;
		printf("회전 전송(회전 = % d)\n", blockData.rotation);
		break;
	case 4: // 스페이스
		blockData.action;
		printf("하드 드롭 전송\n");
		break;
	default:
		break;
	}

	blockData.posX = testX;
	blockData.posY = testY;
	blockData.timestamp = (u_int)time(NULL);

	// 서버로 패킷 전송
	int result = sendTMCP((unsigned int)clientSocket, TMCP_BLOCK_MOVE, &blockData, sizeof(blockData));
	if (result < 0) {
		printf("❌ 패킷 전송 실패!\n");
	}
}


UINT NetworkManager::AcceptServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "WSADATA 생성 실패\n";
		return -1;
	}
	// 서버 연결 정보 입력
	char serverIP[32] = "127.0.0.1"; // 기본은 로컬 IP로
	int serverPort = 5004;
	std::cout << "서버 연결 시도: " << serverIP << ":" << serverPort << "\n";
	// 다른 IP 및 port를 이용할 것이라면 입력 받기
	//char inputIP[32];
	//std::cin >> inputIP;
	//if (strlen(inputIP) > 0)
	//{
	//	strcpy_s(serverIP, sizeof(serverIP), inputIP);
	//}
	//char inputPort[16];
	//gets_s(inputPort, sizeof(inputPort));
	//if (strlen(inputPort) > 0) 
	//{
	//	serverPort = atoi(inputPort);
	//}

	// TCP 소켓 생성
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "소켓 생성 실패\n";
		WSACleanup();
		return -1;
	}

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	serverAddr.sin_port = htons(serverPort);

	// 서버와 연동 시도
	if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cout << "서버 연결 실패\n";
		closesocket(clientSocket);
		WSACleanup();
		return -1;
	}

	// 여기까지 왔다면 서버 연결 성공
	isConnected = true;

	// 연결 성공 REQ 보내기
	sendTMCP((unsigned int)clientSocket, TMCP_CONNECT_REQ, clientName, static_cast<u_short>(strlen(clientName)));

	// 수신 스레드 생성
	receiveHandle = (HANDLE)_beginthreadex(NULL, 0, &NetworkManager::ReceiveThread, NULL, 0, NULL);
	if (!receiveHandle) 
	{
		std::cout << "수신 스레드 생성 실패\n";
		/*closesocket(clientSocket);
		WSACleanup();*/
		Disconnect();
		return -1;
	}

	#pragma region 메인 입력 루프 (블럭 별로 처리하던가 Player를 만들어서 처리해야함) 
	// TODO : 메인 입력 루프 -> 블럭 별로 처리하던가 Player를 만들어서 처리해야함.
	//printf("조작법:\n");
	//printf("방향키: 블록 이동 테스트\n");
	//printf("ESC: 종료\n");
	//printf("다른 클라이언트도 실행해서 서로 패킷을 주고받는지 확인하세요!\n\n");

	//while (isConnected) 
	//{
	//	if (_kbhit())
	//	{
	//		int key = _getch();

	//		// 방향키 처리 (확장 키코드)
	//		if (key == 224) {  // 확장 키 프리픽스
	//			key = _getch();  // 실제 방향키 코드

	//			if (isGameStarted) {
	//				switch (key) {
	//				case 72:  // 위쪽 화살표
	//					SendDirectionKey(0);
	//					break;
	//				case 80:  // 아래쪽 화살표  
	//					SendDirectionKey(3);
	//					break;
	//				case 75:  // 왼쪽 화살표
	//					SendDirectionKey(1);
	//					break;
	//				case 77:  // 오른쪽 화살표
	//					SendDirectionKey(2);
	//					break;
	//				}
	//			}
	//			else 
	//			{
	//				printf("게임이 시작되지 않았습니다. 상대방을 기다려주세요.\n");
	//			}
	//		}

	//		else if (key == VK_SPACE)
	//		{
	//			SendDirectionKey(4);
	//		}
	//		// ESC 키
	//		else if (key == VK_ESCAPE)
	//		{
	//			printf("종료 요청...\n");
	//			sendTMCP((unsigned int)clientSocket, TMCP_DISCONNECT_REQ, NULL, 0);
	//			break;
	//		}
	//	}

	//	Sleep(10);  // CPU 사용률 조절
	//}
#pragma endregion

	Game::GetInstance().StartMultiPlayer();

	// === 정리 ===
	//isConnected = false;

	//if (receiveHandle) {
	//	WaitForSingleObject(receiveHandle, 2000);  // 2초 대기
	//	CloseHandle(receiveHandle);
	//}

	//if (clientSocket != INVALID_SOCKET) {
	//	closesocket(clientSocket);
	//}

	//WSACleanup();

	return 0;
}
