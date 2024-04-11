from gtts import gTTS

import requests
import json
import pygame.mixer
import time

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
time.sleep(5)
