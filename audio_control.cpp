#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
//CAN 통신
#include "sensor_data.h"
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
// GPIO
// #include <wiringPi.h>

#define RECEIVER_IP "192.168.1.11"
#define RECEIVER_PORT 50001
#define BUFFER_SIZE 1024

using namespace std;

// LED TURN ON FUNCTION
// void turnOnLED(int pin) {
//     digitalWrite(pin, HIGH); // LED 켜기
// }

// // LED TURN OFF FUNCTION
// void turnOffLED(int pin) {
//     digitalWrite(pin, LOW); // LED 끄기
// }

vector<pair<string,int>> event;
pid_t ppid = getppid(); //music player pid

int sensor_flag = 0;
int soc;
int readBytes;
struct sockaddr_can addr;
struct can_frame frame;
struct ifreq ifr;

// CAN 네트워크 인터페이스 이름 (예: can0)
const char *ifname = "can0";

void readCANData(int soc) {
    int readBytes;
    struct can_frame frame;

    readBytes = read(soc, &frame, sizeof(struct can_frame));

    if(readBytes) {
        SensorData data;
        data.frame = frame;
        sensorDataList.push_back(data); // 읽은 데이터 저장
    }
}

void sleepCheckHandler(int sig){
    printf("sleepcheckhandler\n");
    event.push_back(pair<string,int> ("sleep",0));
}

void tempCheckHandler(int sig){
   printf("tempcheckhandler\n");
   event.push_back(pair<string,int> ("temp",1));
}

void dustCheckHandler(int sig){
   printf("dustcheckhandler\n");
   event.push_back(pair<string,int> ("dust",2));
}

void soundCheckHandler(int sig){
   printf("soundcheckhandler\n");
   event.push_back(pair<string,int> ("sound",3));
}

void dashboardCheckHandler(int sig){
   printf("dashboardcheckhandler\n");
   //무슨 경고인지 확인해서 event.push 할 것
   event.push_back(pair<string,int> ("dashboard",4));
} 

void sleepDectection() {
    printf("Sleep Detection Start\n");
    if (execlp("python3", "python3", "Sleep_Detection.py", NULL) == -1) {
        perror("execlp"); 
        exit(EXIT_FAILURE);
    }
    // execlp 함수가 성공적으로 실행되면 아래의 코드는 실행되지 않음
    printf("This line won't be executed\n");
}

void sensorDetection(){
    //4가지 센서 값을 받아드리고 기준치 이상인 경우
    printf("Sensor Detection process parent pid : %d\n",getppid());
    pid_t parent_pid = getppid();
    int cnt = 0;

    while(1){
        readCANData(soc);

        cout << (int)sensorDataList[0].frame.data[0] << " " << (int)sensorDataList[0].frame.data[1] << " " << (int)sensorDataList[0].frame.data[2] << " " << (int)sensorDataList[0].frame.data[3] << " " << (int)sensorDataList[0].frame.data[4] << " " << (int)sensorDataList[0].frame.data[5] << "\n";

        //온습도
        if(sensorDataList[0].frame.data[2] >= 30 && sensorDataList[0].frame.data[0] <=10 && sensor_flag != 1){
            printf("Success TEMPERATURE\n");
            sensor_flag = 1;
            kill(parent_pid, SIGRTMIN + 2);
        }

        //소리감지 센서
        // if(sensorDataList[0].frame.data[1] >= 10 && sensor_flag != 2){
        //     printf("Success SOUND\n");
        //     sensor_flag = 2;
        //     kill(parent_pid, SIGRTMIN + 4);
        // }

        // //미세먼지 센서
        // if(sensorDataList[0].frame.data[4] >= 20 && sensor_flag != 3){
        //     printf("Success DUST\n");
        //     sensor_flag = 3;
        //     kill(parent_pid, SIGRTMIN + 3);
        // }

        //계기판 경고인식해서 
        //kill(parent_pid, SIGRTMIN + 5);

        //가변저항 외부라파로 전송하기
        // if(cnt > 100 ){
        //     cnt = 0;
        //     int data = htonl(sensorDataList[sensorDataList.size()-1].frame.data[5]);

        //     struct sockaddr_in receiverAddr;
        //     memset(&receiverAddr, 0, sizeof(receiverAddr));
        //     receiverAddr.sin_family = AF_INET;
        //     receiverAddr.sin_port = htons(RECEIVER_PORT);
        //     receiverAddr.sin_addr.s_addr = inet_addr(RECEIVER_IP);

        //     // 소켓 생성
        //     int senderSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        //     ssize_t bytesSent = sendto(senderSocket, &data, sizeof(data), 0,
        //                         (struct sockaddr*)&receiverAddr, sizeof(receiverAddr));
        //     close(senderSocket);
        // }
        // cnt++;
        sensorDataList.clear();
    }
}

int main() {

    struct sigaction sleepcheck;
    sleepcheck.sa_handler = sleepCheckHandler;
    sigemptyset(&sleepcheck.sa_mask);
    sleepcheck.sa_flags = 0;
    sigaction(SIGUSR1, &sleepcheck, 0);
    
    struct sigaction tempcheck;
    tempcheck.sa_handler = tempCheckHandler;
    sigemptyset(&tempcheck.sa_mask);
    tempcheck.sa_flags = 0;
    sigaction(SIGRTMIN + 2, &tempcheck, 0);

    struct sigaction dustcheck;
    dustcheck.sa_handler = dustCheckHandler;
    sigemptyset(&dustcheck.sa_mask);
    dustcheck.sa_flags = 0;
    sigaction(SIGRTMIN + 3, &dustcheck, 0);

    struct sigaction soundcheck;
    soundcheck.sa_handler = soundCheckHandler;
    sigemptyset(&soundcheck.sa_mask);
    soundcheck.sa_flags = 0;
    sigaction(SIGRTMIN + 4, &soundcheck, 0);

    struct sigaction dashboardcheck;
    dashboardcheck.sa_handler = dashboardCheckHandler;
    sigemptyset(&dashboardcheck.sa_mask);
    dashboardcheck.sa_flags = 0;
    sigaction(SIGRTMIN + 5, &dashboardcheck, 0);

    if((soc = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Error while opening socket");
        return -1;
    }

    strcpy(ifr.ifr_name, ifname);
    ioctl(soc, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error in socket bind");
        return -2;
    }

    bool pressed = false; // 조건에 해당하는 데이터를 발견했는지 여부를 나타내는 플래그

    while(!pressed && (sensorDataList.size() == 0 || sensorDataList[sensorDataList.size()-1].frame.data[0] <= 128)) {
        readCANData(soc);

        // 조건에 해당하는 데이터를 발견한 경우에만 플래그를 설정하고 반복문을 빠져나감
        if (sensorDataList.size() > 0 && sensorDataList[sensorDataList.size()-1].frame.data[0] > 128) {
            pressed = true;
        }
    }

    // 조건에 해당하는 데이터를 발견한 경우에만 "Pressed"를 출력하고 시그널을 보냄
    if (pressed) {
        printf("Pressed\n");
        kill(ppid, SIGRTMIN + 5);
    }
    sensorDataList.clear();

    pid_t sleep_pid = fork();

    if(sleep_pid == 0){
        sleep(3);
        // sleepDectection();
    }else{
        pid_t sensor_pid = fork();

        if(sensor_pid == 0){
            sleep(8);
            sensorDetection();
        }
        else{
            sleep(2);
            printf("Audio schedule process pid : %d\n",getpid());
            while(1){
                if(event.size() != 0){
                    printf("Audio Scheduling Start %d\n",getpid());
                    auto it = find(event.begin(), event.end(), pair<string,int> ("sleep",0));
                    if(it == event.end()){
                        int temp = event.front().second;
                        event.erase(event.begin());
                        switch(temp){
                            case 1:
                                kill(ppid, SIGRTMIN + 2);
                                break;
                            case 2:
                                
                                kill(ppid, SIGRTMIN + 3);
                                break;
                            case 3:
                                kill(ppid, SIGRTMIN + 4);
                                break;
                            case 4:
                                kill(ppid, SIGRTMIN + 5);
                                break;
                        }
                    }
                    else{
                        // SLEEP
                        event.erase(it);
                        kill(ppid, SIGUSR1);
                    }
                }
            }
        }
    }
}