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
    for (const auto& data : sensorDataList) {
        if(data.frame.can_dlc >= 6) { // 데이터 길이 확인
            
            unsigned char pressureValue = data.frame.data[0];
            unsigned char soundValue = data.frame.data[1];
            unsigned char tempValue = data.frame.data[2];
            unsigned char humidValue = data.frame.data[3];
            unsigned char dustValue = data.frame.data[4];
            unsigned char variableregValue = data.frame.data[5];

            cout << (int)pressureValue << " " << (int)soundValue << " " << (int)tempValue << " " << (int)humidValue << " " << (int)dustValue << " " << (int)variableregValue << "\n";
        }
    }
    sensorDataList.clear(); // sensorDataList 비우기, 누적되는 것을 방지하기 위함.
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