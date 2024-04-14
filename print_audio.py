import sys
import os
import signal
import time
import pygame
import RPi.GPIO as GPIO
import socket

receiver_ip = "192.168.1.11"
receiver_port = 50000

sender_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

freq = 44100
bitsize = -16
channels = 2
buffer = 4096

# GPIO 핀 번호 기준 설정
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

ENA = 13 # WiringPi 핀 1은 BCM 핀 18에 해당
IN1 = 19 # WiringPi 핀 2는 BCM 핀 27에 해당
IN2 = 26 # WiringPi 핀 3은 BCM 핀 22에 해당

# 핀 설정
GPIO.setup(ENA, GPIO.OUT)
GPIO.setup(IN1, GPIO.OUT)
GPIO.setup(IN2, GPIO.OUT)

# 모터 회전 방향 제어 함수
def setMotorDirection(direction):
    if direction == 1:
        GPIO.output(IN1, GPIO.HIGH)
        GPIO.output(IN2, GPIO.LOW)
    elif direction == -1:
        GPIO.output(IN1, GPIO.LOW)
        GPIO.output(IN2, GPIO.HIGH)
    else:
        GPIO.output(IN1, GPIO.LOW)
        GPIO.output(IN2, GPIO.LOW)

# PWM 설정
pwm = GPIO.PWM(ENA, 100) # PWM 핀 설정, 100Hz
pwm.start(0) # PWM 시작, 0% 듀티 사이클

# 모터 속도 제어 함수 (0~100)
def setMotorSpeed(speed):
    pwm.ChangeDutyCycle(speed)

os.getppid()
action = sys.argv[1]
print("Arguments:", action)
pygame.mixer.init(freq, bitsize, channels, buffer)

def led_on(pin):
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.HIGH)

def led_off(pin):
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)

if action == "sleep":
    # sleep인 경우 원하는 mp3 파일을 재생합니다.
    message = "sleep"
    sender_socket.sendto(message.encode('utf-8'), (receiver_ip, receiver_port)) 
    time.sleep(1)
    
    led_on(12)
    pygame.mixer.music.load("drowsy_detection.mp3")
    pygame.mixer.music.set_volume(1)
    pygame.mixer.music.play()
    
    
    
    
    
    # 재생이 끝날 때까지 대기
    while pygame.mixer.music.get_busy():
        time.sleep(1)
        
    led_off(12)
    sender_socket.close()
    exit(0)

if action == "temp":
    message = "temp"
    sender_socket.sendto(message.encode('utf-8'), (receiver_ip, receiver_port)) 
    sender_socket.close()
    led_on(16)
    time.sleep(10)
    led_off(16)
    
    exit()

if action == "dust":
    # message = "dust"
    # sender_socket.sendto(message.encode('utf-8'), (receiver_ip, receiver_port)) 
    time.sleep(1)
    pygame.mixer.music.load("dust.mp3")
    pygame.mixer.music.set_volume(1)
    pygame.mixer.music.play()
    
    setMotorDirection(1)
    led_on(20)
    setMotorSpeed(30)
    time.sleep(5)
    led_off(20)
    setMotorDirection(0)
    
    # 재생이 끝날 때까지 대기
    while pygame.mixer.music.get_busy():
        time.sleep(1)
    # sender_socket.close()
    exit(0)

if action == "sound":
    # sleep인 경우 원하는 mp3 파일을 재생합니다.
    led_on(21)
    time.sleep(10)
    led_off(21)
    exit(0)
    
#TTS
