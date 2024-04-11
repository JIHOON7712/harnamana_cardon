import sys
import os
import signal
import time
import pygame


os.getppid()
action = sys.argv[1]
print("Arguments:", action)
pygame.mixer.init()

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
    
    


#TTS
