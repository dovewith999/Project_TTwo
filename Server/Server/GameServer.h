#pragma once

#pragma comment(lib, "ws2_32")

#pragma warning(disable:4996)

//#include <stdio.h> iostream도 포함해놓고 이걸 왜 쓰지? 일단 지워봄 07/31
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <iostream>
#include <vector>
#include <stack>

/// 클라이언트 연결 정보 구조체
struct ClientInfo
{
	SOCKET clientSocket;
	bool isConnect;
	char id[50];
	HANDLE clientHandle;
	HANDLE listenHandle;

	ClientInfo()
	{
		memset(id, 0, sizeof(id));
		clientSocket = NULL;
		isConnect = false;
		clientHandle = NULL;
		listenHandle = NULL;
	}
};

// 타입 재정의
using ClientIterator = std::vector<ClientInfo>::iterator;

class GameServer
{
public:
	GameServer();
	~GameServer();

public:
	void Wait();
	void Listen(int portNum);
	static UINT WINAPI ListenThread(LPVOID p);
	static UINT WINAPI ControlThread(LPVOID p);

private:
	int InitSocketLayer();
	void CloseSocketLayer();

private:
	int portNum = 0;
	SOCKET serverSocket = NULL;
	HANDLE listenHandle = NULL;
	HANDLE mainHandle = NULL;
	bool isRun = true;
	SOCKET lastSocket = NULL;
	std::vector<ClientInfo> clientList; // 클라이언트들의 연결 정보
	std::stack<ClientIterator, std::vector<ClientIterator>> clientPool; // 연결된 소켓을 풀링 구조로 관리하기 위한 스택
};

