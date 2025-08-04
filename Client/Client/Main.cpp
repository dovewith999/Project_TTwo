//#pragma comment(lib, "ws2_32")
//#pragma warning(disable:4996)
//
//#include<stdio.h>
//#include<stdlib.h>
//#include<WinSock2.h>
//#include <process.h>
//#include <conio.h>
//
//#define LEFT  75
//#define RIGHT 77
//#define UP    72
//#define DOWN  80
//
//unsigned _stdcall ClientRun(void* p);
//bool g_IsRunning;
//
//int main(int argc, char** agrv)
//{
//    WSADATA wsadata;
//    SOCKET Sock;
//    SOCKADDR_IN serverAddr;
//    int portnum;
//    char message[100];
//    char serverIP[20];
//    portnum = 5004;
//    int len;
//    HANDLE hThread;
//
//    printf("=== 멀티 클라이언트 채팅 프로그램 ===\n");
//
//    // 서버 IP 입력받기
//    printf("연결할 서버 IP를 입력하세요.\n");
//    printf("(로컬 테스트: 127.0.0.1, LAN 내 다른 PC: 192.168.x.x 형태)\n");
//    printf("서버 IP: ");
//    gets_s(serverIP, sizeof(serverIP));
//
//    // 빈 입력시 기본값으로 로컬호스트 사용
//    if (strlen(serverIP) == 0) {
//        strcpy_s(serverIP, sizeof(serverIP), "127.0.0.1");
//        printf("기본값 사용: %s\n", serverIP);
//    }
//
//    printf("포트 번호 (기본값: 5004, 엔터로 기본값 사용): ");
//    char portInput[10];
//    gets_s(portInput, sizeof(portInput));
//
//    if (strlen(portInput) > 0) {
//        portnum = atoi(portInput);
//        if (portnum <= 0 || portnum > 65535) {
//            printf("잘못된 포트 번호입니다. 기본값 5004를 사용합니다.\n");
//            portnum = 5004;
//        }
//    }
//
//    printf("\n연결 시도 중... (%s:%d)\n", serverIP, portnum);
//
//    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
//    {
//        printf("WSAStartup Error\n");
//        return -1;
//    }
//
//    Sock = socket(PF_INET, SOCK_STREAM, 0);
//    if (Sock == INVALID_SOCKET)
//    {
//        printf("socket 생성 실패\n");
//        return -1;
//    }
//
//    memset(&serverAddr, 0, sizeof(serverAddr));
//    serverAddr.sin_family = AF_INET;
//    serverAddr.sin_port = htons(portnum);
//    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
//
//    // IP 주소 유효성 검사
//    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
//        printf("잘못된 IP 주소 형식입니다: %s\n", serverIP);
//        closesocket(Sock);
//        WSACleanup();
//        return -1;
//    }
//
//    if (connect(Sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
//    {
//        printf("서버 연결 실패!\n");
//        printf("서버가 실행 중인지, IP 주소와 포트가 올바른지 확인하세요.\n");
//        printf("에러 코드: %d\n", WSAGetLastError());
//        closesocket(Sock);
//        WSACleanup();
//        return -1;
//    }
//
//    printf("서버 연결 성공!\n");
//    printf("채팅을 시작합니다. 'exit'를 입력하면 종료됩니다.\n");
//    printf("==========================================\n\n");
//
//    hThread = (HANDLE)_beginthreadex(NULL, 0, ClientRun, (void*)Sock, CREATE_SUSPENDED, NULL);
//
//    if (!hThread)
//    {
//        printf("스레드 생성 에러!\n");
//        closesocket(Sock);
//        WSACleanup();
//        return -1;
//    }
//
//    g_IsRunning = true;
//    ResumeThread(hThread);
//
//    while (g_IsRunning)
//    {
//        if (_kbhit()) {
//            char key = _getch();
//            bool shouldSend = false;
//
//            if (key == -32) {    // 확장 키 (방향키)
//                key = _getch();
//                switch (key) {
//                case 75: // LEFT
//                    strcpy_s(message, "왼쪽으로 이동");
//                    shouldSend = true;
//                    break;
//                case 77: // RIGHT
//                    strcpy_s(message, "오른쪽으로 이동");
//                    shouldSend = true;
//                    break;
//                case 72: // UP
//                    strcpy_s(message, "회전");
//                    shouldSend = true;
//                    break;
//                case 80: // DOWN
//                    strcpy_s(message, "아래로 이동");
//                    shouldSend = true;
//                    break;
//                }
//            }
//            else if (key == ' ') {    // 스페이스바
//                strcpy_s(message, "하드드롭");
//                shouldSend = true;
//            }
//            else if (key == 27) {     // ESC
//                strcpy_s(message, "exit");
//                shouldSend = true;
//            }
//
//            if (shouldSend) {
//                send(Sock, message, (int)strlen(message), 0);
//                if (strcmp(message, "exit") == 0)
//                    break;
//                memset(message, 0, sizeof(message));
//            }
//        }
//
//        Sleep(1); // CPU 사용률 조절
//    }
//
//    g_IsRunning = false; // 수신 스레드도 종료하도록 설정
//    WaitForSingleObject(hThread, INFINITE);
//    CloseHandle(hThread);
//
//    closesocket(Sock);
//    WSACleanup();
//
//    printf("연결이 종료되었습니다.\n");
//    printf("아무 키나 누르면 프로그램이 종료됩니다...");
//    getchar();
//
//    return 0;
//}
//
//unsigned _stdcall ClientRun(void* p)
//{
//    SOCKET Sock = (SOCKET)p;
//    char recvMessage[100];
//    int len;
//
//    while (g_IsRunning == true)
//    {
//        memset(recvMessage, 0, sizeof(recvMessage));
//        len = recv(Sock, recvMessage, sizeof(recvMessage) - 1, 0);
//
//        if (len == SOCKET_ERROR)
//        {
//            if (g_IsRunning) { // 정상 종료가 아닌 경우만 에러 출력
//                printf("\n[수신 오류] 서버와의 연결이 끊어졌습니다.\n");
//            }
//            g_IsRunning = false;
//            break;
//        }
//
//        if (len == 0) {
//            printf("\n[알림] 서버가 연결을 종료했습니다.\n");
//            g_IsRunning = false;
//            break;
//        }
//
//        recvMessage[len] = '\0';
//
//        if (strcmp(recvMessage, "exit") == 0)
//        {
//            printf("\n[알림] 서버에서 연결을 종료합니다.\n");
//            g_IsRunning = false;
//            break;
//        }
//
//        // 메시지 앞에 줄바꿈을 추가하여 입력 프롬프트와 구분
//        printf("\n>> %s\n", recvMessage);
//        printf("메시지 입력: "); // 프롬프트 다시 표시
//        fflush(stdout); // 즉시 출력
//    }
//
//    return 0;
//}

#include <iostream>
#include "Engine.h"
#include "Game/Game.h"
#include "SoundManager.h"
#include "Manager/FileManager.h"

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Game game;
    game.Run();

    return 0;
}