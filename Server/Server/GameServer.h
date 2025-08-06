#pragma once

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

//#include <stdio.h> iostream도 포함해놓고 이걸 왜 쓰지? 일단 지워봄 07/31
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <vector>
#include <stack>
#include <mutex>
#include "TMCP.h"

// 게임 세션 구조체
struct GameSession
{
	SOCKET player1Socket;
	SOCKET player2Socket;

	char player1Name[20];
	char player2Name[20];

	bool isActive;
	time_t startTime;

	HANDLE player1Handle;
	HANDLE player2Handle;

	GameSession()
	{
		player1Socket = INVALID_SOCKET;
		player2Socket = INVALID_SOCKET;

		memset(player1Name, 0, sizeof(player1Name));
		memset(player2Name, 0, sizeof(player2Name));

		isActive = false;
		startTime = 0;

		player1Handle = NULL;
		player2Handle = NULL;
	}

	void Reset()
	{
		if (player1Socket != INVALID_SOCKET) closesocket(player1Socket);
		if (player2Socket != INVALID_SOCKET) closesocket(player2Socket);
		if (player1Handle) { CloseHandle(player1Handle); player1Handle = NULL; }
		if (player2Handle) { CloseHandle(player2Handle); player2Handle = NULL; }

		player1Socket = INVALID_SOCKET;
		player2Socket = INVALID_SOCKET;
		isActive = false;
	}
};

// 플레이어 스레드 매개변수
struct PlayerParam
{
	class GameServer* server;
	SOCKET playerSocket;
	SOCKET opponentSocket;
	int playerId;

	PlayerParam(GameServer* server, SOCKET player, SOCKET opponent, int id)
	: server(server), playerSocket(player), opponentSocket(opponent), playerId(id) {}
};

///// 클라이언트 연결 정보 구조체
//struct ClientInfo
//{
//	SOCKET clientSocket;
//	bool isConnect;
//	char id[50];
//	HANDLE clientHandle;
//	HANDLE listenHandle;
//
//	ClientInfo()
//	{
//		memset(id, 0, sizeof(id));
//		clientSocket = NULL;
//		isConnect = false;
//		clientHandle = NULL;
//		listenHandle = NULL;
//	}
//};

// 타입 재정의
//using ClientIterator = std::vector<ClientInfo>::iterator;

class GameServer
{
public:
	GameServer();
	~GameServer();

	// === 서버 제어 ===
	void Listen(int portNumber);  // 서버 시작
	void Wait();                  // 서버 대기

	// === 정보 조회  ===
	bool IsGameActive() { return currentSession.isActive; }
	void PrintStatus();           // 서버 상태 출력

	// === 정적 스레드 함수 ===
	static UINT WINAPI AcceptThread(LPVOID param);    // 연결 수락 스레드 
	static UINT WINAPI PlayerThread(LPVOID param);    // 플레이어 처리 스레드 (Non-blocking)

private:
	// === 기본 서버 함수 ===
	int  InitSocketLayer();
	void CloseSocketLayer();

	// === 게임 시작/종료 ===
	void StartGame(SOCKET player1, SOCKET player2);  // 게임 시작
	void EndGame();                                  // 게임 종료 및 정리

	// === 패킷 중계 ===
	void RelayPacket(SOCKET from, SOCKET to, u_char cmd, void* payload, int size);

private:
	// === 기본 서버 정보 ===
	int          portNumber;     // 서버 포트
	SOCKET       serverSocket;   // 서버 소켓
	bool         isRunning;      // 서버 실행 상태

	// === 게임 세션 ===
	GameSession  currentSession; // 현재 게임 세션
	std::mutex   sessionMutex;   // 세션 동기화

	// === 연결 대기 소켓들 ===
	SOCKET       waitingSocket;  // 대기 중인 첫 번째 클라이언트 소켓
	std::mutex   waitingMutex;   // 대기 소켓 동기화

	// === 통계 정보 ===
	time_t       serverStartTime; // 서버 시작 시간
};

// ========================================
// 전역 함수
// ========================================
void SendTMCPPacket(SOCKET socket, u_char cmd, void* payload = NULL, u_short len = 0);
void PrintGameStart(const char* player1, const char* player2);
void PrintGameEnd(const char* winner);

