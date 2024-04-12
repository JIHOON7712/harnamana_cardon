#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>

#include <wiringPi.h>
#include <string>

using namespace std;

int speed;
#define RECEIVER_PORT 50001

#define trigPin 1	//gpio 21
#define echoPin 29	//gpio 18

#define SERVO1 26

void pedestrianCheckHandler(int sig){
    //초음파 센서로 거리 측정하고
    //속도에 따라서 제동 거리 계산(90-110db로 제동거리를 scale)
    //보행자 거리를 측정해서 제동 거리 내에 있으면 즉각 가상 배기음 출력
    
    int distance=0;
	int pulse = 0;
	
	long startTime;
	long travelTime;
	
	if(wiringPiSetup () == -1)
	{
		printf("Unable GPIO Setup"); 
	}
		
	pinMode (trigPin, OUTPUT);
	pinMode (echoPin, INPUT);
	
    int flag = 0;
	for(;;)
	{
		digitalWrite (trigPin, LOW);
		usleep(2);
		digitalWrite (trigPin, HIGH);
		usleep(20);
		digitalWrite (trigPin, LOW);
		
		while(digitalRead(echoPin) == LOW);
		
		startTime = micros();
		
		while(digitalRead(echoPin) == HIGH);
		travelTime = micros() - startTime;
		
		int distance = travelTime / 58;
        if(speed <= 70 && distance <= 10){
            flag = 1;
            int volumePercentage = 70 / distance;
            const char* mp3FilePath = "exhaust_sound.mp3";
            string command = "amixer -D pulse sset Master " + to_string(volumePercentage) + "&";
            system(command.c_str());
            system(("mpg123 " + string(mp3FilePath)).c_str());
        }
        if(flag == 1) break;
		printf( "Distance: %dcm\n", distance);
		delay(200);
	}

}

int main(){
    pid_t camera = fork();
    if(camera == 0){
        
        if(execlp("/usr/bin/python3", "python3", "pedestrian_detection.py", NULL) == -1) {
            perror("execlp"); // 오류 메시지 출력
            exit(EXIT_FAILURE); // 실패 시 자식 프로세스 종료
        }
    }
    else{
        pid_t checksleepdust = fork();
            if(checksleepdust == 0){
                if (execlp("./window_control", "window_control", NULL) == -1) {
                perror("execlp"); // 오류 메시지 출력
                exit(EXIT_FAILURE); // 실패 시 자식 프로세스 종료
            }
        }
        else{
            sleep(1);
            printf("%d\n",getpid());
            struct sigaction pedestriancheck;
            pedestriancheck.sa_handler = pedestrianCheckHandler;
            sigemptyset(&pedestriancheck.sa_mask);
            pedestriancheck.sa_flags = 0;
            sigaction(SIGUSR1, &pedestriancheck, 0);

            while(true){
                //내부 라즈베리파이로부터 값을 받아오기
                int receiverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                struct sockaddr_in receiverAddr;
                memset(&receiverAddr, 0, sizeof(receiverAddr));
                receiverAddr.sin_family = AF_INET;
                receiverAddr.sin_port = htons(RECEIVER_PORT);
                receiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
                bind(receiverSocket, (struct sockaddr*)&receiverAddr, sizeof(receiverAddr));

                // 데이터 수신
                struct sockaddr_in senderAddr;
                socklen_t senderAddrLen = sizeof(senderAddr);
                ssize_t bytesReceived = recvfrom(receiverSocket, &received_data, sizeof(received_data), 0,
                                                    (struct sockaddr*)&senderAddr, &senderAddrLen);

                // 수신된 데이터를 네트워크 바이트 순서에서 호스트 바이트 순서로 변환
                received_data = ntohl(received_data);
            }
            close(receiverSocket);
        }
    }
}