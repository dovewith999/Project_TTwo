//#pragma comment(lib, "ws2_32")
//#pragma warning(disable:4996)
//
//#include<stdio.h>
//#include<stdlib.h>
//#include<WinSock2.h>
//#include <process.h>
//#include <vector>
//
//#define MAX_CLIENTS 10
//
//struct ClientInfo {
//	SOCKET socket;
//	int id;
//	bool isConnected; // 연결 상태를 확인하기 위한 변수
//	bool isReady; // 게임을 시작하기 위한 변수
//};
//
//std::vector<ClientInfo> g_Clients;
//CRITICAL_SECTION g_CS; // 클라이언트 벡터 동기화용
//bool g_IsRunning;
//int g_ClientIdCounter = 1;
//
//unsigned _stdcall ClientHandler(void* p);
//void BroadcastMessage(const char* message, int senderId = -1);
//void RemoveClient(int clientId);
//
//int main(int argc, char** agrv)
//{
//	WSADATA wsadata;
//	SOCKET ServSock;
//	SOCKADDR_IN serverAddr, cliAddr;
//	int portnum = 5004;
//	int szCliAddr;
//	char sendMessage[100];
//
//	// Critical Section 초기화
//	InitializeCriticalSection(&g_CS);
//
//	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
//	{
//		printf("WSAStartup Error\n");
//		return -1;
//	}
//
//	ServSock = socket(PF_INET, SOCK_STREAM, 0);
//	if (ServSock == INVALID_SOCKET)
//	{
//		printf("socket 생성 실패\n");
//		return -1;
//	}
//
//	memset(&serverAddr, 0, sizeof(serverAddr));
//	serverAddr.sin_family = AF_INET;
//	serverAddr.sin_port = htons(portnum);
//	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//
//	if (bind(ServSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
//	{
//		printf("Bind Error\n");
//		return -1;
//	}
//
//	if (listen(ServSock, SOMAXCONN) == SOCKET_ERROR)
//	{
//		printf("Listen error\n");
//		return -1;
//	}
//
//	printf("서버가 포트 %d에서 시작되었습니다.\n", portnum);
//	printf("최대 %d명의 클라이언트를 지원합니다.\n", MAX_CLIENTS);
//	printf("'broadcast [메시지]'로 모든 클라이언트에게 메시지 전송\n");
//	printf("'list'로 연결된 클라이언트 목록 확인\n");
//	printf("'exit'로 서버 종료\n\n");
//
//	g_IsRunning = true;
//
//	// 클라이언트 연결 수락 스레드
//	HANDLE acceptThread = (HANDLE)_beginthreadex(NULL, 0, [](void* param) -> unsigned {
//		SOCKET ServSock = *(SOCKET*)param;
//		SOCKADDR_IN cliAddr;
//		int szCliAddr;
//
//		while (g_IsRunning)
//		{
//			szCliAddr = sizeof(cliAddr);
//			SOCKET CliSock = accept(ServSock, (SOCKADDR*)&cliAddr, &szCliAddr);
//
//			if (CliSock == INVALID_SOCKET)
//			{
//				if (g_IsRunning) // 정상적인 종료가 아닌 경우에만 에러 출력
//					printf("Accept 에러!\n");
//				continue;
//			}
//
//			EnterCriticalSection(&g_CS);
//			if (g_Clients.size() >= MAX_CLIENTS)
//			{
//				LeaveCriticalSection(&g_CS);
//				printf("최대 클라이언트 수 초과! 연결 거부\n");
//				closesocket(CliSock);
//				continue;
//			}
//
//			ClientInfo newClient;
//			newClient.socket = CliSock;
//			newClient.id = g_ClientIdCounter++;
//			newClient.isConnected = true;
//			newClient.isReady = false;
//			g_Clients.push_back(newClient);
//
//			int clientId = newClient.id;
//			LeaveCriticalSection(&g_CS);
//
//			printf("클라이언트 %d 연결됨! (IP: %s)\n", clientId, inet_ntoa(cliAddr.sin_addr));
//
//			// 각 클라이언트마다 별도의 핸들러 스레드 생성
//			HANDLE clientThread = (HANDLE)_beginthreadex(NULL, 0, ClientHandler,
//				(void*)(intptr_t)clientId, 0, NULL);
//			if (!clientThread)
//			{
//				printf("클라이언트 %d 스레드 생성 실패!\n", clientId);
//				RemoveClient(clientId);
//			}
//			else
//			{
//				CloseHandle(clientThread); // 스레드 핸들은 바로 닫아도 스레드는 계속 실행됨
//			}
//		}
//		return 0;
//		}, &ServSock, 0, NULL);
//
//	// 서버 명령어 처리 루프
//	while (g_IsRunning)
//	{
//		printf("> ");
//		gets_s(sendMessage, sizeof(sendMessage));
//
//		if (strcmp(sendMessage, "exit") == 0)
//		{
//			g_IsRunning = false;
//			BroadcastMessage("서버가 종료됩니다.");
//			break;
//		}
//		else if (strcmp(sendMessage, "list") == 0)
//		{
//			EnterCriticalSection(&g_CS);
//			printf("연결된 클라이언트: %d명\n", (int)g_Clients.size());
//			for (const auto& client : g_Clients)
//			{
//				if (client.isConnected)
//					printf("  - 클라이언트 ID: %d\n", client.id);
//			}
//			LeaveCriticalSection(&g_CS);
//		}
//		else if (strncmp(sendMessage, "broadcast ", 10) == 0)
//		{
//			BroadcastMessage(sendMessage + 10);
//			printf("모든 클라이언트에게 메시지 전송: %s\n", sendMessage + 10);
//		}
//		else
//		{
//			printf("사용 가능한 명령어:\n");
//			printf("  broadcast [메시지] - 모든 클라이언트에게 메시지 전송\n");
//			printf("  list - 연결된 클라이언트 목록\n");
//			printf("  exit - 서버 종료\n");
//		}
//
//		memset(sendMessage, 0, sizeof(sendMessage));
//	}
//
//	// 정리 작업
//	WaitForSingleObject(acceptThread, INFINITE);
//	CloseHandle(acceptThread);
//
//	// 모든 클라이언트 소켓 닫기
//	EnterCriticalSection(&g_CS);
//	for (auto& client : g_Clients)
//	{
//		if (client.isConnected)
//			closesocket(client.socket);
//	}
//
//	g_Clients.clear();
//	LeaveCriticalSection(&g_CS);
//
//	closesocket(ServSock);
//	WSACleanup();
//	DeleteCriticalSection(&g_CS);
//
//	printf("서버가 종료되었습니다.\n");
//	return 0;
//}
//
//unsigned _stdcall ClientHandler(void* p)
//{
//	int clientId = (int)(intptr_t)p;
//	SOCKET clientSocket = INVALID_SOCKET;
//
//	// 클라이언트 정보 찾기
//	EnterCriticalSection(&g_CS);
//	for (const auto& client : g_Clients)
//	{
//		if (client.id == clientId && client.isConnected)
//		{
//			clientSocket = client.socket;
//			break;
//		}
//	}
//	LeaveCriticalSection(&g_CS);
//
//	if (clientSocket == INVALID_SOCKET)
//	{
//		printf("클라이언트 %d 소켓을 찾을 수 없습니다.\n", clientId);
//		return 0;
//	}
//
//	char recvMessage[100];
//	char broadcastMsg[150];
//	int len;
//
//	while (g_IsRunning)
//	{
//		memset(recvMessage, 0, sizeof(recvMessage));
//		len = recv(clientSocket, recvMessage, sizeof(recvMessage) - 1, 0);
//
//		if (len == SOCKET_ERROR || len == 0)
//		{
//			printf("클라이언트 %d 연결 종료 (recv 에러)\n", clientId);
//			break;
//		}
//
//		recvMessage[len] = '\0';
//
//		if (strcmp(recvMessage, "exit") == 0)
//		{
//			printf("클라이언트 %d가 연결을 종료했습니다.\n", clientId);
//			break;
//		}
//
//		printf("[클라이언트 %d] %s\n", clientId, recvMessage);
//
//		// 다른 모든 클라이언트에게 메시지 브로드캐스트
//		snprintf(broadcastMsg, sizeof(broadcastMsg), "[클라이언트 %d] %s", clientId, recvMessage);
//		BroadcastMessage(broadcastMsg, clientId);
//	}
//
//	RemoveClient(clientId);
//	return 0;
//}
//
//void BroadcastMessage(const char* message, int senderId)
//{
//	EnterCriticalSection(&g_CS);
//
//	for (auto it = g_Clients.begin(); it != g_Clients.end();)
//	{
//		if (it->isConnected && it->id != senderId) // 보낸 사람 제외
//		{
//			int result = send(it->socket, message, (int)strlen(message), 0);
//			if (result == SOCKET_ERROR)
//			{
//				printf("클라이언트 %d로의 전송 실패\n", it->id);
//				closesocket(it->socket);
//				it = g_Clients.erase(it);
//			}
//			else
//			{
//				++it;
//			}
//		}
//		else
//		{
//			++it;
//		}
//	}
//
//	LeaveCriticalSection(&g_CS);
//}
//
//void RemoveClient(int clientId)
//{
//	EnterCriticalSection(&g_CS);
//
//	for (auto it = g_Clients.begin(); it != g_Clients.end(); ++it)
//	{
//		if (it->id == clientId)
//		{
//			if (it->isConnected)
//			{
//				closesocket(it->socket);
//				it->isConnected = false;
//			}
//			g_Clients.erase(it);
//			break;
//		}
//	}
//
//	LeaveCriticalSection(&g_CS);
//	printf("클라이언트 %d 제거됨\n", clientId);
//}

#include "GameServer.h"

int main()
{
	GameServer server;
	server.Listen(5004);
	server.Wait();
	return 0;
}