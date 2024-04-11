#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define RECEIVER_IP "192.168.1.11"
#define RECEIVER_PORT 50001

int main() {
    // 송신 데이터
    const char* message = "Hello from Raspberry Pi 2!";

    // 수신측 주소 설정
    struct sockaddr_in receiverAddr;
    memset(&receiverAddr, 0, sizeof(receiverAddr));
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(RECEIVER_PORT);
    receiverAddr.sin_addr.s_addr = inet_addr(RECEIVER_IP);

    // 소켓 생성
    int senderSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (senderSocket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // 데이터 송신
    while(1){
        ssize_t bytesSent = sendto(senderSocket, message, strlen(message), 0,
                                    (struct sockaddr*)&receiverAddr, sizeof(receiverAddr));
        if (bytesSent < 0) {
            std::cerr << "Failed to send data" << std::endl;
            close(senderSocket);
            return 1;
        }
        sleep(3);
    }
    

    // 소켓 닫기
    close(senderSocket);

    return 0;
}
