// can_read.cpp
#include "sensor_data.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <vector>

using namespace std;

// vector<SensorData> sensorDataList; // 전역 변수 정의

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

void processSensorData() {
    // 여기서 sensorDataList를 처리
    for (const auto& data : sensorDataList) {
        // 가정: frame.data에는 5개의 센서 값이 저장되어 있음
        if(data.frame.can_dlc >= 5) { // 데이터 길이 확인
            for (int i = 0; i < 5; i++) { // 5개 센서 값 처리
                unsigned char sensorValue = data.frame.data[i]; // 센서 값 추출
                cout << "Sensor " << i + 1 << ": " << (int)sensorValue << "\n"; // 센서 값 출력
                // 여기서 각 센서 값에 대한 추가 처리를 수행할 수 있음
            }
        }
    }
}


int main() {
    int soc;
    int readBytes;
    struct sockaddr_can addr;
    struct can_frame frame;
    struct ifreq ifr;

    // CAN 네트워크 인터페이스 이름 (예: can0)
    const char *ifname = "can0";

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

    while(1) {
        readCANData(soc);
        processSensorData();
    }

    return 0;
}