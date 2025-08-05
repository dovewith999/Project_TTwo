#pragma once
#include "Etc/Singleton.h"
#include "Protocol/TMCP.h"

/// <summary>
/// 서버와의 통신을 담당하는 매니저 클래스
/// 서버로 데이터를 보내고 받는 기능을 한다.
/// 작성자 : 임희섭
/// 작성일 : 25/08/04
/// </summary>
class NetworkManager final : public SafeSingleton<NetworkManager>
{
	friend class SafeSingleton<NetworkManager>;

public:
	explicit NetworkManager() = default;
	virtual ~NetworkManager() = default;

public:
	void SendInput(int input); // 입력을 서버로 보내는 함수
	UINT AcceptServer(); // 서버와 연결을 시도하는 함수
	static UINT WINAPI ReceiveThread(LPVOID param);

public:
	FORCEINLINE bool GetIsConnected() const { return isConnected; }

private:
	SOCKET clientSocket = INVALID_SOCKET;
	bool isConnected = false;
	bool isGameStarted = false;
	char clientName[32] = "OwnerClient";
};