#include <DHT11.h>
#include <SPI.h>
#include <mcp_can.h>

/* const macro */
#define TEMP_HUMID_PIN 2
#define SOUND_PIN 0
#define PRESSURE_PIN 7
#define DUST_LED_PIN 4
#define DUST_PIN 3
#define POTENTIOMETER_PIN 5

/* sensor variable */
int pressure; // 압력
int sound; // 소리
int temp; // 온도
int humid; // 습도
float dust_raw; // 원시 미세먼지 값
int dust; // 미세먼지
int dust_sampling = 280; // 적외선 led를 키고, 센서 값을 읽어 들여 미세먼지를 측정하는 샘플링 시간
int dust_waiting = 40; // 
float dust_stop_time = 9680; // 미세먼지 센서를 구동하지 않는 시간
int potentio = 0; // 가변저항 값

/* DTH11 tempHumid sensor init */
DHT11 dht11(TEMP_HUMID_PIN); // 온습도 센서 모듈 초기화

/* CAN init */
const int SPI_CS_PIN = 53; // SPI CS 핀 설정
MCP_CAN CAN(SPI_CS_PIN); // MCP_CAN 인스턴스 생성

void setup() {
  Serial.begin(115200);
  pinMode(DUST_LED_PIN, OUTPUT);  // 미세먼지센서 내부 적외선 led를 출력으로 설정
  
  /* can module init */
  while (CAN_OK != CAN.begin(CAN_500KBPS, MCP_8MHz)) { // 500kbps 속도로 CAN 통신 시작
    Serial.println("CAN BUS Shield init failed");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
}

void loop() {
  Serial.println("Sending");
  unsigned char stmp[8] = {0,}; // CAN으로 보낼 8바이트 데이터 배열 초기화
  
  /* get sensor value */
  getPressure(); // 압력
  getSound(); // 소리
  getTempHumid(); // 온습도
  getDust(); // 미세먼지
  getPotentio(); // 가변저항

  /* set CAN FRMAE */
  stmp[0] = map(pressure, 0, 1023, 0, 255); //압력
  stmp[1] = map(sound,0,1023,0,255); //소리
  stmp[2] = temp; // 온도
  stmp[3] = humid; // 습도
  stmp[4] = dust; // 미세먼지
  stmp[5] = map(potentio, 0, 1023, 0, 255); // 가변저항

  /* check CAN FRAME */
  // Serial.println(stmp[0]);
  // Serial.println(stmp[1]);
  // Serial.println(stmp[2]);
  // Serial.println(stmp[3]);
  // Serial.println(stmp[4]);
  // Serial.println(stmp[5]);
  // Serial.println("");

  
  /* send CAN FRAME*/
  CAN.sendMsgBuf(0x100, 0, 6, stmp); // CAN 메시지 ID는 0x100, 표준 프레임, 6바이트 데이터 전송, 보낼 CAN FRAME DATA
  delay(1000);
}

  /* get sensor value function */
  void getPressure(){
    pressure = analogRead(PRESSURE_PIN);
    delay(500);
  }

  void getSound(){
    sound = analogRead(SOUND_PIN);
  }

  void getTempHumid(){
    int result = dht11.readTemperatureHumidity(temp, humid); // result는 함수 실패 or 성공 결과
  }

  void getPotentio(){
    potentio = analogRead(POTENTIOMETER_PIN);
  }

  void getDust(){
    digitalWrite(DUST_LED_PIN, LOW); // 내부 적외선 LED ON
    delayMicroseconds(dust_sampling);  

    dust_raw = analogRead(DUST_PIN); // 아날로그 핀 값 읽어옴
    delayMicroseconds(dust_waiting); // 핀 값 읽기 위한 시간

    digitalWrite(DUST_LED_PIN, HIGH);  // 내부 적외선 LED OFF
    delay(500);

    dust = (int)(((0.17 * (dust_raw * (5.0 / 1024)) - 0.1) * 1000) + 100); // raw 값을 [ug/m3] 차원으로 계산, 음수 값 처리 위해 +100
  }

  /* for debug function */
  void printPressure(){
    Serial.print("PRESSURE     : ");
    Serial.println(pressure);
  }

    void printSound(){
    Serial.print("SOUND        : ");
    Serial.println(sound);
  }

  void printTemp(){
    Serial.print("Temperature  : "); 
    Serial.println(temp);
  }

  void printHumid(){
    Serial.print("Humidity     : "); 
    Serial.println(humid);
  }
  
  void printDust(){
    Serial.print("Dust Density : "); 
    Serial.println(dust);
  }

  void printPotentio(){
    Serial.print("Potenio      : ");
    Serial.println(potentio);
  }
