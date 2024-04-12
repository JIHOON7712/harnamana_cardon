import sys
import os
import signal
import time
import pygame
import RPi.GPIO as GPIO

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
pygame.mixer.init()

def led_on(pin):
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.HIGH)

def led_off(pin):
    GPIO.setup(pin, GPIO.OUT)
    GPIO.output(pin, GPIO.LOW)

if action == "sleep":
    # sleep인 경우 원하는 mp3 파일을 재생합니다.
    pygame.mixer.music.load("wake_up.mp3")
    pygame.mixer.music.play()
    # 재생이 끝날 때까지 대기
    while pygame.mixer.music.get_busy():
        time.sleep(1)

if action == "temp":
    # 모터 제어
    # temp인 경우 오디오 출력
    try:
        led_on(16)
        time.sleep(5)
        led_off(16)
        
        setMotorDirection(1)
        setMotorSpeed(80)
        time.sleep(10)
        setMotorDirection(0)
    
        # pygame.mixer.music.load("wake_up.mp3")
        # pygame.mixer.music.play()
        # 재생이 끝날 때까지 대기
        while pygame.mixer.music.get_busy():
            time.sleep(1)
    finally:
        pwm.stop()
        GPIO.cleanup()

if action == "dust":
    # sleep인 경우 원하는 mp3 파일을 재생합니다.
    pygame.mixer.music.load("wake_up.mp3")
    pygame.mixer.music.play()
    # 재생이 끝날 때까지 대기
    while pygame.mixer.music.get_busy():
        time.sleep(1)

if action == "sound":
    # sleep인 경우 원하는 mp3 파일을 재생합니다.
    pygame.mixer.music.load("wake_up.mp3")
    pygame.mixer.music.play()
    # 재생이 끝날 때까지 대기
    while pygame.mixer.music.get_busy():
        time.sleep(1)

if action == "dashboard":
    # sleep인 경우 원하는 mp3 파일을 재생합니다.
    pygame.mixer.music.load("wake_up.mp3")
    pygame.mixer.music.play()
    # 재생이 끝날 때까지 대기
    while pygame.mixer.music.get_busy():
        time.sleep(1)
    
#TTS
