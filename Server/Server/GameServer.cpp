#include "GameServer.h"
#include <sys\types.h>
#include <sys\stat.h>
#include <mmsystem.h>

GameServer::GameServer()
    : portNum(0), isRun(true)
{
    InitSocketLayer(); // WSA 소켓 초기화 작업을 여기서 진행

    for (int i = 0; i < 100; ++i)
    {
        ClientInfo info;
        clientList.emplace_back(info);
    }

    ClientIterator iter;
    iter = clientList.begin();
    while (iter != clientList.end())
    {
        clientPool.push(iter);
        ++iter;
    }
}

GameServer::~GameServer()
{
    // 데이터 정리
    isRun = false;

    while (!clientPool.empty())
    {
        clientPool.pop();
    }

    ClientIterator iter;
    iter = clientList.begin();
    while (iter != clientList.end()) 
    {
        if (iter->isConnect == true) // 아직 client와 연결된 상태라면 연결을 종료시킴
        {
            WaitForSingleObject(iter->clientHandle, INFINITE);
            CloseHandle(iter->clientHandle);
            closesocket(iter->clientSocket);
            ++iter;
        }
    }

}

void GameServer::Wait()
{
    WaitForSingleObject(listenHandle, INFINITE);
}

void GameServer::Listen(int portNum)
{
    this->portNum = portNum;
    listenHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, GameServer::ListenThread, this, 0, NULL));

    if (!listenHandle)
    {
        std::cout << "Error : [Listen()] listenHandle == 0\m";
        return;
    }
}

UINT __stdcall GameServer::ListenThread(LPVOID p)  
{  
    GameServer* server;  
    server = static_cast<GameServer*>(p);  
    server->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  

    if (server->serverSocket == INVALID_SOCKET)  
    {  
        WSACleanup();  
        return -1;  
    }  

    sockaddr_in service;  
    memset(&service, 0, sizeof(service));  

    service.sin_family = AF_INET;  
    service.sin_addr.s_addr = htonl(INADDR_ANY);  
    service.sin_port = htons(server->portNum);  

    // Correct the type casting and remove the undefined "serverAddr".  
    if (bind(server->serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR)  
    {  
        closesocket(server->serverSocket);
        return -1;  
    }  

    if (listen(server->serverSocket, 5) == SOCKET_ERROR)  
    {  
        closesocket(server->serverSocket);
        return -1;  
    }  

    while (server->isRun)
    {
        SOCKET connectSocket;
        connectSocket = accept(server->serverSocket, NULL, NULL);
        if (connectSocket > 0)
        {
            if (server->clientPool.empty())
            {
                closesocket(connectSocket);
                continue;
            }

            else
            {
                ClientIterator iter;
                iter = server->clientPool.top(); // socket pooling
                server->clientPool.pop();         
                iter->clientSocket = connectSocket;
                server->lastSocket = connectSocket;
                iter->isConnect = true;
                iter->clientHandle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, GameServer::ControlThread, server, 0, NULL));
            }
        }
    }

    return 0;  
}

UINT __stdcall GameServer::ControlThread(LPVOID p)
{
    return 0;
}

int GameServer::InitSocketLayer()
{
    int retval = 0;
    WORD verRequest = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(verRequest, &wsaData))
    {
        printf("WSAStartup Error\n");
        return -1;
    }

    return 0;
}

void GameServer::CloseSocketLayer()
{
    WSACleanup();
}
