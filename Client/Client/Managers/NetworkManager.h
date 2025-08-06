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
	virtual ~NetworkManager();

public:
	void SendInput(class TetrisBlock* block, int input, bool isFixed = false); // 입력을 서버로 보내는 함수
	void SendPacket(int pktMsg); // TODO : short로 받아도 될 것 같다.
	void SendAttackLines(const TMCPAttackData& attackData);
	//void SendDirectionKey(int direction); // 테스트용 함수
	UINT AcceptServer(); // 서버와 연결을 시도하는 함수
	static UINT WINAPI ReceiveThread(LPVOID param);

	void Disconnect(); // 연결이 종료되면 처리할 일들

public: /*Getter*/
	FORCEINLINE bool GetIsConnected() const { return isConnected; }	
	FORCEINLINE bool GetIsGameStarted() const { return isGameStarted; }
	FORCEINLINE bool GetIsWaitingForOpponent() const { return isWaitingForOpponent; }

private:
	SOCKET clientSocket = INVALID_SOCKET;
	bool isConnected = false;
	bool isGameStarted = false;
	bool isWaitingForOpponent = false;
	char clientName[32] = "OwnerClient";

	HANDLE receiveHandle; // 수신 스레드를 관리할 핸들

};