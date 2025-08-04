#pragma once
#include "Etc/Singleton.h"
#include "Protocol/TMCP.h"

class NetworkManager : public SafeSingleton<NetworkManager>
{
	friend class SafeSingleton<NetworkManager>;
	friend UINT WINAPI ReceiveThread(LPVOID param); // __beginthreadex에서 넘기려면 멤버 함수면 안됨..

public:
	explicit NetworkManager() = default;
	virtual ~NetworkManager() = default;

public:
	void SendInput(int input); // 입력을 서버로 보내는 함수
	UINT AcceptServer(); // 서버와 연결을 시도하는 함수

private:
	SOCKET clientSocket = INVALID_SOCKET;
	bool isConnected = false;
	bool isGameStarted = false;
	char clientName[32] = "OwnerClient";
};

UINT WINAPI ReceiveThread(LPVOID param); // 서버로부터 데이터를 받을 함수