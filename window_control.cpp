#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <wiringPi.h>
#include "softPwm.h"

using namespace std;

#define RECEIVER_PORT 50000
#define BUFFER_SIZE 1024
#define SERVO1 26

void servo_setup() { // 서보모터 셋업
    wiringPiSetup(); // WiringPi 초기화
    pinMode(SERVO1, OUTPUT);
}

void window_open(){ // 창문 열기
    softPwmCreate(SERVO1, 0, 200);
    
    softPwmWrite(SERVO1, 25);   // +90도
    delay(1000);
}

void window_close(){ // 창문 닫기
    softPwmCreate(SERVO1, 0, 200);
    
    softPwmWrite(SERVO1, 5);   // 0도
    delay(1000);
}

int main() {
    servo_setup();
    char buffer[BUFFER_SIZE];

    // 소켓 생성
    int receiverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (receiverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // 수신 소켓에 바인딩
    struct sockaddr_in receiverAddr;
    memset(&receiverAddr, 0, sizeof(receiverAddr));
    receiverAddr.sin_family = AF_INET;
    receiverAddr.sin_port = htons(RECEIVER_PORT);
    receiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(receiverSocket, (struct sockaddr*)&receiverAddr, sizeof(receiverAddr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        close(receiverSocket);
        return 1;
    }

    // 데이터 수신 및 출력
    struct sockaddr_in senderAddr;
    socklen_t senderAddrLen = sizeof(senderAddr);
    ssize_t bytesReceived = recvfrom(receiverSocket, buffer, BUFFER_SIZE, 0,
                                     (struct sockaddr*)&senderAddr, &senderAddrLen);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving data" << std::endl;
        close(receiverSocket);
        return 1;
    }

    std::cout << "수신된 데이터: " << buffer << std::endl;

    string data(buffer,4);
    window_open();
    sleep(3);

    if(data == string("temp")){
        const char* mp3FilePath = "emergency.mp3";
        system("amixer -D pulse sset Master 80%");
        system(("mpg123 " + string(mp3FilePath)).c_str());
    }

    window_close();

    // 소켓 닫기
    close(receiverSocket);

    return 0;
}
