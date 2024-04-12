#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include "softPwm.h"

#define PORT1 50000
#define PORT2 50001
#define BUFFER_SIZE 1024

void servo_setup() { // 서보모터 셋업
    wiringPiSetup(); // WiringPi 초기화
    pinMode(SERVO1, OUTPUT);
}

void window_open(){ // 창문 열기
    softPwmCreate(SERVO1, 0, 200);
    // softPwmWrite(SERVO1, 10);   // -90도
    // delay(1000);
    softPwmWrite(SERVO1, 15);   // 0도
    delay(1000);
    softPwmWrite(SERVO1, 20);   // +90도
    delay(1000);
}

void window_close(){ // 창문 닫기
    softPwmCreate(SERVO1, 0, 200);
    // softPwmWrite(SERVO1, 10);   // -90도
    // delay(1000);
    softPwmWrite(SERVO1, 20);   // +90도
    delay(1000);
    softPwmWrite(SERVO1, 15);   // 0도
    delay(1000);
}

int main() {
    int sockfd1, sockfd2;
    char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    struct sockaddr_in serverAddr1, serverAddr2;
    socklen_t addr_size;

    // 소켓1 생성
    sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
    // 소켓2 생성
    sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);

    // 서버 주소1 설정
    memset(&serverAddr1, '\0', sizeof(serverAddr1));
    serverAddr1.sin_family = AF_INET;
    serverAddr1.sin_port = htons(PORT1);
    serverAddr1.sin_addr.s_addr = INADDR_ANY;

    // 서버 주소2 설정
    memset(&serverAddr2, '\0', sizeof(serverAddr2));
    serverAddr2.sin_family = AF_INET;
    serverAddr2.sin_port = htons(PORT2);
    serverAddr2.sin_addr.s_addr = INADDR_ANY;

    // 소켓1과 서버 주소1 바인딩
    bind(sockfd1, (struct sockaddr*)&serverAddr1, sizeof(serverAddr1));

    // 소켓2과 서버 주소2 바인딩
    bind(sockfd2, (struct sockaddr*)&serverAddr2, sizeof(serverAddr2));

    // 소켓을 비블록으로 설정
    fcntl(sockfd1, F_SETFL, O_NONBLOCK);
    fcntl(sockfd2, F_SETFL, O_NONBLOCK);

    // 데이터 수신
    fd_set readfds;
    int maxfd = (sockfd1 > sockfd2) ? sockfd1 + 1 : sockfd2 + 1;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(sockfd1, &readfds);
        FD_SET(sockfd2, &readfds);

        // select() 함수를 사용하여 소켓 이벤트 감지
        int activity = select(maxfd, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            std::cerr << "Error in select" << std::endl;
            break;
        }

        if (FD_ISSET(sockfd1, &readfds)) {
            // 소켓1에서 데이터 수신
            addr_size = sizeof(serverAddr1);
            int bytes_received1 = recvfrom(sockfd1, buffer1, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddr1, &addr_size);
            // 수신된 데이터 출력
            std::cout << "Received message on PORT1: " << buffer2 << std::endl;
        }

        if (FD_ISSET(sockfd2, &readfds)) {
            // 소켓2에서 데이터 수신
            addr_size = sizeof(serverAddr2);
            int bytes_received2 = recvfrom(sockfd2, buffer2, BUFFER_SIZE, 0, (struct sockaddr*)&serverAddr2, &addr_size);
            // 수신된 데이터 출력
            std::cout << "Received message on PORT2: " << buffer2 << std::endl;
        }
    }

    // 소켓1 닫기
    close(sockfd1);
    // 소켓2 닫기
    close(sockfd2);

    return 0;
}
