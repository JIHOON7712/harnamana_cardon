# 음악 프로그램 실행
import os
import signal
import multiprocessing
import time
import pygame
import subprocess
import requests
import json

lock = multiprocessing.Lock()

freq = 44100
bitsize = -16
channels = 2
buffer = 4096

def sigusr1_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        child_process = multiprocessing.Process(target=sleep_child_process)
        child_process.start()
        child_process.join()
        pygame.mixer.music.set_volume(1.0)

def sleep_child_process():
    os.execlp("python3", "python3", "print_audio.py", "sleep")

def temp_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        child_process = multiprocessing.Process(target=temp_child_process)
        child_process.start()
        child_process.join()
        pygame.mixer.music.set_volume(1.0)

def temp_child_process():
    os.execlp("python3", "python3", "print_audio.py", "temp")

def dust_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        child_process = multiprocessing.Process(target=dust_child_process)
        child_process.start()
        child_process.join()
        pygame.mixer.music.set_volume(1.0)

def dust_child_process():
    os.execlp("python3", "python3", "print_audio.py", "dust")

def sound_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        sound_process = subprocess.Popen(["python3", "print_audio.py", "sound"])
        sound_process.wait()  # 자식 프로세스의 종료를 기다림
        pygame.mixer.music.set_volume(1.0)

def dashboard_handler(signum, frame):
    with lock:
        pass

signal.signal(signal.SIGRTMIN + 5, dashboard_handler)

#날씨 정보 출력
def play_background_music(music_file):
    # 초기화
    pygame.mixer.init(freq, bitsize, channels, buffer)
    # 배경음악 로드 및 재생 (반복 재생)
    pygame.mixer.music.load(music_file)
    pygame.mixer.music.play(-1)  # -1을 전달하여 반복 재생을 설정합니다.

signal.sigwait([signal.SIGRTMIN + 5])

signal.signal(signal.SIGUSR1, sigusr1_handler)
signal.signal(signal.SIGRTMIN + 2, temp_handler)
signal.signal(signal.SIGRTMIN + 3, dust_handler)
signal.signal(signal.SIGRTMIN + 4, sound_handler)

file = "result_files/weather.mp3"

pygame.mixer.init(freq, bitsize, channels, buffer)
pygame.mixer.music.load(file)
pygame.mixer.music.play()
time.sleep(6)

music_file = "sunlit-whistle-200168.mp3"
play_background_music(music_file)

while(1):
    
    pass
