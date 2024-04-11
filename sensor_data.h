/ sensor_data.h
#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <vector>
#include <linux/can.h>

using namespace std;

struct SensorData {
    can_frame frame; // CAN 프레임 데이터
};

vector<SensorData> sensorDataList; // 센서 데이터 목록

void readCANData(int soc); // CAN 데이터 읽기 함수
void processSensorData(); // 센서 데이터 처리 함수

#endif