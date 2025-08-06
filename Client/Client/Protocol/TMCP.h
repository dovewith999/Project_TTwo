#pragma once
/// <summary>
/// 사용자 정의 프로토콜 제작
/// TMCP : Tetris Multi-Client Protocol
/// 작성자 : 임희섭
/// 작성일 : 25/08/04
/// </summary>
#ifndef _TMCP_1_0_0_H
#define _TMCP_1_0_0_H

#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>

#define WIN32 1

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <io.h>
#endif

#ifndef WIN32
#include <sys/socket.h>
#include <unistd.h>
#endif

#define TMCP_VER				0x10
#define TMCP_VERSION_CURRENT	TMCP_VER
#define TMCP_MAX_PAYLOAD_SIZE	1024


// 프로토콜 연결 관리
#define TMCP_CONNECT_REQ		0x01
#define TMCP_CONNECT_RES		0x02
#define TMCP_DISCONNECT_REQ		0x03
#define TMCP_MATCH_WAIT			0x04
#define TMCP_MATCH_FOUND		0x05

// 게임 상태
#define TMCP_GAME_START			0x10
#define TMCP_GAME_OVER			0x11
#define TMCP_GAME_PAUSE			0x12
#define TMCP_GAME_RESUME		0x13

// 테트리스 게임 액션
#define TMCP_BLOCK_MOVE			0x20
#define TMCP_BLOCK_ROTATE		0x21
#define TMCP_BLOCK_DROP			0x22
#define TMCP_BLOCK_HARD_DROP	0x23

// 게임 이벤트
#define TMCP_LINE_CLEAR			0x30
#define TMCP_SCORE_UPDATE		0x31
#define TMCP_ATTACK_LINES		0x32
#define TMCP_NEXT_BLOCK			0x33

// 기타	
#define TMCP_HERATBEAT // 연결 확인

#ifdef WIN32
using u_char = unsigned char;
using u_int = unsigned int;
using u_short = unsigned short;
#endif

typedef struct TMCPHeader_t
{
	u_char version;			// 프로토콜 버전
	u_char cmd;				// 명령어
	u_short packetLength;	// 전체 패킷 길이 (헤더 + 페이로드)
} TMCPHeader;

// 블록 이동 데이터 (12 bytes) - 실용적이고 확장 가능한 크기
typedef struct TMCPBlockData_t {
    u_char blockType;        // 블록 타입 (I, O, T, S, Z, J, L = 1~7)
    u_char rotation;         // 회전 상태 (0~3)
    u_char action;           // 동작 (0:이동, 1:회전, 2:고정, 3:즉시낙하)
    u_char direction;        // 이동 방향 (0:정지, 1:좌, 2:우, 3:아래)
    u_short posX;            // X 좌표 (확장 가능, 큰 보드 지원)
    u_short posY;            // Y 좌표 (확장 가능, 큰 보드 지원)
    u_int timestamp;         // 타임스탬프 (동기화용)
} TMCPBlockData;

// 라인 클리어 데이터 (16 bytes) - 충분한 정보 저장
typedef struct TMCPLineData_t {
    u_char linesCount;       // 클리어된 라인 수 (1~4)
    u_char lineNumbers[4];   // 클리어된 라인 번호들 (0~19)
    u_char attackLines;      // 상대방에게 보낼 공격 라인 수
    u_char comboCount;       // 현재 콤보 수
    u_char spinType;         // T-spin 등 특수 동작 (0:일반, 1:T-spin, 2:Perfect Clear)
    u_char reserved;         // 예약
    u_int scoreGained;       // 이번 클리어로 얻은 점수
} TMCPLineData;

// 점수 업데이트 데이터 (16 bytes) - 테트리스 점수는 매우 커질 수 있음
typedef struct TMCPScoreData_t {
    u_int  currentScore;     // 현재 점수 (최대 42억점)
    u_int  totalLines;       // 총 클리어 라인 수 (최대 42억 라인)
    u_short currentLevel;    // 현재 레벨 (1~65535)
    u_short fallSpeed;       // 현재 낙하 속도 (ms 단위)
    u_int timestamp;         // 타임스탬프 (게임 시간)
} TMCPScoreData;


/**
 * TMCP 패킷 전송
 * @param sockfd 소켓
 * @param cmd 명령어
 * @param payload 데이터 (NULL 가능)
 * @param len 데이터 크기
 * @return 전송된 바이트 수 (실패시 음수)
 */
int sendTMCP(unsigned int sockfd, unsigned char cmd, void* payload, unsigned short len);

/**
 * TMCP 패킷 수신
 * @param sockfd 소켓
 * @param cmd 수신된 명령어 (출력)
 * @param payload 수신 데이터 버퍼
 * @param maxLen 버퍼 최대 크기
 * @return 수신된 페이로드 크기 (실패시 음수)
 */
int recvTMCP(unsigned int sockfd, unsigned char* cmd, void* payload, unsigned short maxLen);

#endif

