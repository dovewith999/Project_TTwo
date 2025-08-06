#include "TMCP.h"

// ========================================
// TMCP 패킷 전송 함수
// ========================================
int sendTMCP(unsigned int sockfd, unsigned char cmd, void* payload, unsigned short len)
{
    TMCPHeader_t header;
    char* packet;
    u_int packetSize;
    int result;

    // 패킷 크기 계산
    packetSize = len + sizeof(TMCPHeader_t);

    // 크기 체크 (간단히)
    if (packetSize > TMCP_MAX_PAYLOAD_SIZE + sizeof(TMCPHeader_t)) {
        printf("[TMCP] 패킷 크기 초과: %d bytes\n", packetSize);
        return -1;
    }

    // 헤더 설정
    header.version = TMCP_VERSION_CURRENT;
    header.cmd = cmd;
    header.packetLength = packetSize;

    // 패킷 메모리 할당
    packet = (char*)malloc(packetSize);
    if (packet == NULL) {
        printf("[TMCP] 메모리 할당 실패\n");
        return -1;
    }

    // 패킷 조립
    memset(packet, 0, packetSize);
    memcpy(packet, &header, sizeof(TMCPHeader_t));

    if (payload != NULL && len > 0) {
        memcpy(packet + sizeof(TMCPHeader_t), payload, len);
    }

    // 전송!
    result = send(sockfd, packet, packetSize, 0);

    // 메모리 해제
    free(packet);

    return result;
}

// ========================================
// TMCP 패킷 수신 함수
// ========================================
int recvTMCP(unsigned int sockfd, unsigned char* cmd, void* payload, unsigned short maxLen)
{
    TMCPHeader_t header;
    char buffer[TMCP_MAX_PAYLOAD_SIZE + sizeof(TMCPHeader_t)];
    int totalReceived = 0;
    int remaining;
    int currentRecv;
    int payloadSize;

    // 매개변수 체크
    if (cmd == NULL || payload == NULL) 
    {
        return -1;
    }

    // 1단계: 헤더만 먼저 받기 (MSG_PEEK)
    int headerRecv = recv(sockfd, (char*)&header, sizeof(TMCPHeader_t), MSG_PEEK);
    if (headerRecv <= 0) 
    {
        return headerRecv;  // 연결 끊김 또는 오류
    }

    // 간단한 헤더 검증
    if (header.packetLength < sizeof(TMCPHeader_t) ||
        header.packetLength > sizeof(buffer))
    {
        printf("[TMCP] 잘못된 패킷 크기: %d\n", header.packetLength);
        return -1;
    }

    // 2단계: 전체 패킷 받기
    remaining = header.packetLength;

    while (totalReceived < header.packetLength) 
    {
        currentRecv = recv(sockfd, buffer + totalReceived, remaining, 0);

        if (currentRecv <= 0) 
        {
            return -1;  // 수신 실패
        }

        totalReceived += currentRecv;
        remaining -= currentRecv;
    }

    // 3단계: 헤더 다시 추출 (실제 데이터에서)
    memcpy(&header, buffer, sizeof(TMCPHeader_t));
    *cmd = header.cmd;

    // 4단계: 페이로드 추출
    payloadSize = header.packetLength - sizeof(TMCPHeader_t);

    if (payloadSize > 0)
    {
        int copySize = (payloadSize > maxLen) ? maxLen : payloadSize;
        memcpy(payload, buffer + sizeof(TMCPHeader_t), copySize);

        if (payloadSize > maxLen) 
        {
            printf("[TMCP] 페이로드 크기 초과, 일부만 복사: %d->%d\n", payloadSize, maxLen);
        }
    }

    return payloadSize;
}