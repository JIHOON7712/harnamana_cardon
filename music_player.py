# 음악 프로그램 실행
import os
import signal
import multiprocessing
import time
import pygame

from gtts import gTTS

import requests
import json
import pygame.mixer

lock = multiprocessing.Lock()

def sigusr1_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        child_process = multiprocessing.Process(target=sleep_child_process)
        child_process.start()
        # 자식 프로세스의 종료를 대기합니다.
        child_process.join()
        # 자식 프로세스가 종료되면 락을 해제합니다.
        pygame.mixer.music.set_volume(1.0)

def sleep_child_process():
    # 자식 프로세스가 해야 할 작업을 수행합니다.
    os.execlp("python3", "python3", "print_audio.py", "sleep")

def temp_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        child_process = multiprocessing.Process(target=temp_child_process)
        child_process.start()
        # 자식 프로세스의 종료를 대기합니다.
        child_process.join()
        # 자식 프로세스가 종료되면 락을 해제합니다.
        pygame.mixer.music.set_volume(1.0)

def temp_child_process():
    # 자식 프로세스가 해야 할 작업을 수행합니다.
    os.execlp("python3", "python3", "print_audio.py", "temp")

def dust_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        child_process = multiprocessing.Process(target=dust_child_process)
        child_process.start()
        # 자식 프로세스의 종료를 대기합니다.
        child_process.join()
        # 자식 프로세스가 종료되면 락을 해제합니다.
        pygame.mixer.music.set_volume(1.0)

def dust_child_process():
    # 자식 프로세스가 해야 할 작업을 수행합니다.
    os.execlp("python3", "python3", "print_audio.py", "dust")

def sound_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        child_process = multiprocessing.Process(target=sound_child_process)
        child_process.start()
        # 자식 프로세스의 종료를 대기합니다.
        child_process.join()
        # 자식 프로세스가 종료되면 락을 해제합니다.
        pygame.mixer.music.set_volume(1.0)

def sound_child_process():
    # 자식 프로세스가 해야 할 작업을 수행합니다.
    os.execlp("python3", "python3", "print_audio.py", "sound")

def dashboard_handler(signum, frame):
    with lock:
        pygame.mixer.music.set_volume(0.2)
        child_process = multiprocessing.Process(target=dashboard_child_process)
        print(os.getpid())
        child_process.start()
        # 자식 프로세스의 종료를 대기합니다.
        child_process.join()
        # 자식 프로세스가 종료되면 락을 해제합니다.
        pygame.mixer.music.set_volume(1.0)

def dashboard_child_process():
    # 자식 프로세스가 해야 할 작업을 수행합니다.
    os.execlp("python3", "python3", "print_audio.py", "dashboard")

signal.signal(signal.SIGUSR1, sigusr1_handler)
signal.signal(signal.SIGRTMIN + 2, temp_handler)
signal.signal(signal.SIGRTMIN + 3, dust_handler)
signal.signal(signal.SIGRTMIN + 4, sound_handler)
signal.signal(signal.SIGRTMIN + 5, dashboard_handler)

#날씨 정보 출력
def play_background_music(music_file):
    # 초기화
    pygame.mixer.init()
    # 배경음악 로드 및 재생 (반복 재생)
    pygame.mixer.music.load(music_file)
    pygame.mixer.music.play(-1)  # -1을 전달하여 반복 재생을 설정합니다.

# api 정보 - 위치, api키, 언어, api call
city = "Seoul"
apikey = "9f9f5c4467a39fb49cc89cdbf0b3857e"
lang = "kr"
api = f"https://api.openweathermap.org/data/2.5/weather?q={city}&appid={apikey}&lang={lang}&units=metric"

# api 호출 결과
result = requests.get(api)
data = json.loads(result.text)

# 현재 날씨, 현재 온도 정보 파싱
weather_description = data['weather'][0]["description"]
current_temp = str(round(data["main"]["temp"]))

# tts 스크립트
scripts = "현재 날씨는 " + weather_description + ". 현재 기온은 " + current_temp + "도 입니다."
# scripts = "타이어의 공기압이 낮습니. 가까운 정비소를 방문하여 점검을 받아주세요"

tts = gTTS(scripts, lang="ko")
file = "result_files/weather.mp3"
tts.save(file)

pygame.mixer.init()
pygame.mixer.music.load(file)
pygame.mixer.music.play()
time.sleep(6)

music_file = "sunlit-whistle-200168.mp3"
play_background_music(music_file)

while(1):
    
    pass