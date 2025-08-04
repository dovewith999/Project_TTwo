#include "GameServer.h"

GameServer::GameServer()
	: portNumber(0), isRunning(true), serverSocket(INVALID_SOCKET), waitingSocket(INVALID_SOCKET), serverStartTime(time(NULL))
{
	if (InitSocketLayer() != 0)
	{
		std::cout << "[Error] : 소켓 초기화 실패\n";
	}
}

GameServer::~GameServer()
{
	isRunning = false;
	EndGame();
	if (waitingSocket != INVALID_SOCKET)
	{
		closesocket(waitingSocket);
	}

	if (serverSocket != INVALID_SOCKET)
	{
		closesocket(serverSocket);
	}

	CloseSocketLayer();

	// 실행 시간 계산 및 출력
	time_t runTime = time(NULL) - serverStartTime;
	std::cout << "실행시간: " << runTime;
}

void GameServer::Listen(int portNumber)
{
	this->portNumber = portNumber;

	HANDLE acceptHandle = (HANDLE)_beginthreadex(NULL, 0, GameServer::AcceptThread, this, 0, NULL);
	if (!acceptHandle)
	{
		return;
	}

	CloseHandle(acceptHandle);
	std::cout << "클라이언트 대기 시작\n";
}

void GameServer::Wait()
{
	while (isRunning)
	{
		Sleep(1000);
	}
}

void GameServer::PrintStatus()
{
}

// 연결 수락 쓰레드
UINT __stdcall GameServer::AcceptThread(LPVOID param)
{
	GameServer* server = (GameServer*)param;
	server->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server->serverSocket == INVALID_SOCKET)
	{
		std::cout << "서버 소켓 생성 실패\n";
		return -1;
	}

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(server->portNumber);

	if (bind(server->serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		return -1;
	}


	if (listen(server->serverSocket, 2) == SOCKET_ERROR)
	{
		return -1;
	}

	std::cout << "포트 " << server->portNumber << "에서 대기중\n";

	while (server->isRunning)
	{
		SOCKET clientSocket = accept(server->serverSocket, NULL, NULL);

		if (clientSocket == INVALID_SOCKET)
		{
			if (server->isRunning)
			{
				std::cout << "accept 실패\n";
			}
			continue;
		}

		// === 첫 번째 클라이언트인지 확인 ===
		std::lock_guard<std::mutex> waitingLock(server->waitingMutex);

		if (server->waitingSocket == INVALID_SOCKET)
		{
			server->waitingSocket = clientSocket;
			SendTMCPPacket(clientSocket, TMCP_MATCH_WAIT);
		}

		else
		{
			SOCKET player1 = server->waitingSocket;
			SOCKET player2 = clientSocket;

			server->waitingSocket = INVALID_SOCKET;

			server->StartGame(player1, player2);
		}
	}

	return 0;
}

UINT __stdcall GameServer::PlayerThread(LPVOID param)
{
	PlayerParam* playerParam = (PlayerParam*)param;
	GameServer* server = playerParam->server;
	SOCKET mySocket = playerParam->playerSocket;
	SOCKET opponentSocket = playerParam->opponentSocket;
	int playerId = playerParam->playerId;

	delete playerParam;

	fd_set fdRead, fdError, fdMaster;
	struct timeval timeOut;
	u_char command;
	char payload[TMCP_MAX_PAYLOAD_SIZE];

	FD_ZERO(&fdMaster);
	FD_SET(mySocket, &fdMaster);

	while (server->isRunning && server->currentSession.isActive)
	{
		fdRead = fdMaster;
		fdError = fdMaster;
		// 타임아웃 시간을 매우 짧게 : 거의 실시간
		timeOut.tv_sec = 0;
		timeOut.tv_usec = 1000; // 1 ms

		int result = select(static_cast<int>(mySocket + 1), &fdRead, NULL, &fdError, &timeOut);

		if (result < 0)
		{
			std::cout << "Select 오휴\n";
			break;
		}

		if (FD_ISSET(mySocket, &fdError))
		{
			std::cout << "소켓 에러 fdError\n";
			break;
		}

		if (FD_ISSET(mySocket, &fdRead))
		{
			memset(payload, 0, sizeof(payload));
			int payloadSize = recvTMCP((u_int)mySocket, &command, payload, sizeof(payload));

			if (payloadSize < 0) {
				printf("[Non-blocking 스레드] Player%d 연결 끊김\n", playerId + 1);
				break;
			}


			// === 특별 처리가 필요한 명령어들 ===
			if (command == TMCP_DISCONNECT_REQ || command == TMCP_GAME_OVER) {
				printf("[게임 종료] Player%d가 게임을 종료했습니다.\n", playerId + 1);

				// 상대방에게 게임 종료 알림
				SendTMCPPacket(opponentSocket, TMCP_GAME_OVER);
				break;
			}

			server->RelayPacket(mySocket, opponentSocket, command, payload, payloadSize);
		}
	}

	server->EndGame();

	return 0;
}

int GameServer::InitSocketLayer()
{
	// Winsock 초기화
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return -1;
	}

	return 0;
}

void GameServer::CloseSocketLayer()
{
	WSACleanup();
}

void GameServer::StartGame(SOCKET player1, SOCKET player2)
{
	std::lock_guard<std::mutex> sessionLeck(sessionMutex);

	// 이미 게임이 진행 중이면 거부
	if (currentSession.isActive)
	{
		closesocket(player1);
		closesocket(player2);
		return;
	}

	currentSession.player1Socket = player1;
	currentSession.player2Socket = player2;
	currentSession.isActive = true;
	currentSession.startTime = time(NULL);
	strcpy_s(currentSession.player1Name, "Player1");
	strcpy_s(currentSession.player2Name, "Player2");

	SendTMCPPacket(player1, TMCP_MATCH_FOUND);
	SendTMCPPacket(player2, TMCP_MATCH_FOUND);

	SendTMCPPacket(player1, TMCP_GAME_START);
	SendTMCPPacket(player2, TMCP_GAME_START);

	PlayerParam* param1 = new PlayerParam(this, player1, player2, 0);
	PlayerParam* param2 = new PlayerParam(this, player2, player1, 1);

	currentSession.player1Handle = (HANDLE)_beginthreadex(NULL, 0, GameServer::PlayerThread, param1, 0, NULL);
	currentSession.player2Handle = (HANDLE)_beginthreadex(NULL, 0, GameServer::PlayerThread, param2, 0, NULL);

	if (!currentSession.player1Handle || !currentSession.player2Handle)
	{
		std::cout << "플레이어 스레드 생성 실패\n";
		EndGame();
		return;
	}
}

void GameServer::EndGame()
{
	std::lock_guard<std::mutex> sessionLock(sessionMutex);

	if (!currentSession.isActive)
	{
		return;
	}
	time_t gametiem = time(NULL) - serverStartTime;
	currentSession.Reset();
}

void GameServer::RelayPacket(SOCKET from, SOCKET to, u_char cmd, void* payload, int size)
{
	int result = sendTMCP((u_int)to, cmd, payload, size);

	if (result < 0)
	{
		std::cout << "패킷 전송 실패\n";
	}
}

void SendTMCPPacket(SOCKET socket, u_char cmd, void* payload, u_short len)
{
	sendTMCP((u_int)socket, cmd, payload, len);
}

void PrintGameStart(const char* player1, const char* player2)
{
	printf("\n🎮 Non-blocking 게임 시작: %s vs %s\n", player1, player2);
}

void PrintGameEnd(const char* winner)
{
	printf("🏆 Non-blocking 게임 종료! 승자: %s\n\n", winner);
}