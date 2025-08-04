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
    listenHandle = (HANDLE)_beginthreadex(NULL, 0, GameServer::ListenThread, this, 0, NULL);

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
    if (bind(server->serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
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
                iter->clientHandle = (HANDLE)_beginthreadex(NULL, 0, GameServer::ControlThread, server, 0, NULL);
            }
        }
        Sleep(50);
    }

    return 0;  
}

UINT __stdcall GameServer::ControlThread(LPVOID p)
{
    GameServer* server = nullptr;
    server = (GameServer*)p;
    SOCKET connectSocket = server->lastSocket;
    char recvMessage[100];
    int len = 0;
    bool bFound = false;

    //소켓 정보를 가지고 어떤 클라이언트인지 알아야 함
    ClientIterator iter;
    iter = server->clientList.begin();

    while (iter != server->clientList.end())
    {
        if (((ClientInfo)(*iter)).clientSocket == connectSocket)
        {
            bFound = true;
            char message[] = "Server Connect";
            send(connectSocket, message, sizeof(message), 0);
            std::cout << "매칭 성공! 클라이언트 연결 소켓: " << iter->clientSocket << "내부 관리 번호: " << iter->id << '\n';
            break;
        }

        ++iter;
    }

    //논 블록킹 서버를 위한 Select 설정들
    fd_set fdReadSet, fdErrorSet, fdMaster;
    struct timeval tvs;

    FD_ZERO(&fdMaster);
    FD_SET(connectSocket, &fdMaster);
    tvs.tv_sec = 0;
    tvs.tv_usec = 100;

    while (iter->isConnect && bFound)
    {
        fdReadSet = fdMaster;
        fdErrorSet = fdMaster;
        select((int)connectSocket + 1, &fdReadSet, NULL, &fdErrorSet, &tvs);

        if (FD_ISSET(connectSocket, &fdReadSet))
        {
            memset(recvMessage, 0, sizeof(recvMessage));
            len = recv(connectSocket, recvMessage, sizeof(recvMessage) - 1, 0);
            if (len == SOCKET_ERROR)
            {
                std::cout << "recv Error" << '\n';
                break;
            }

            recvMessage[len] = '\0';

            if (strcmp(recvMessage, "exit") == 0)
            {
                std::cout << "소켓 연결을 종료합니다.\n";
                iter->isConnect = false;
                break;
            }

            std::cout << "클라이언트에서 받은 메시지 : " << recvMessage;

            //여기서 접속한 모든 클라이언트들에게 메세지를 보냄
            ClientIterator clientIter;
            clientIter = server->clientList.begin();
            while (clientIter != server->clientList.end())
            {
                if (clientIter->isConnect)
                {
                    std::cout << "메세지 재전송 : 클라이언트 연결 소켓: " << clientIter->clientSocket << "내부 관리 번호: " << clientIter->id << '\n';
                    send(clientIter->clientSocket, recvMessage, static_cast<int>(strlen(recvMessage)), 0);
                }

                ++clientIter;
            }
        }
    }

    closesocket(iter->clientSocket);
    server->clientPool.push(iter);
    std::cout << "클라이언트 연결 종료\n";

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
