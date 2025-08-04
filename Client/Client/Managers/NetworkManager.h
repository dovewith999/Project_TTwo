#pragma once
#include "Etc/Singleton.h"
#include "Protocol/TMCP.h"

class NetworkManager : public SafeSingleton<NetworkManager>
{
	friend class SafeSingleton<NetworkManager>;
	friend UINT WINAPI ReceiveThread(LPVOID param); // __beginthreadex에서 넘기려면 멤버 함수면 안됨..

public:
	explicit NetworkManager();
	virtual ~NetworkManager() = default;

public:
	void SendInput(int input);
	UINT AcceptServer();

private:
	SOCKET clientSocket = INVALID_SOCKET;
	bool isConnected = false;
	bool isGameStarted = false;
	char clientName[32] = "OwnerClient";
};

UINT WINAPI ReceiveThread(LPVOID param);