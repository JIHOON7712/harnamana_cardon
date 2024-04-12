#보행자 감지 코드

import cv2
import random
import sys
import os
import signal
import time

# 동영상 불러오기
cap=cv2.VideoCapture(0)

if not cap.isOpened():
    print('Video open failed!')
    sys.exit()
    
# 보행자 검출을 위한 HOG 기술자 설정
hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

human_detecting_counter = 0

while True:
    ret, frame = cap.read()

    cv2.line(frame, (140, 0), (140, 480), (255, 0, 0) ,2)
    cv2.line(frame, (500, 0), (500, 480), (255, 0, 0) ,2)
    
    if not ret:
        break
        
    # 매 프레임마다 보행자 검출
    detected, _ = hog.detectMultiScale(frame) # 사각형 정보를 받아옴
    
    # 검출 결과 화면 표시
    for (x, y, w, h) in detected:
        c = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

        if 180 <= x <= 400:
            cv2.rectangle(frame, (x, y, w, h), c, 3)

            human_detecting_counter = human_detecting_counter + 1
            print(human_detecting_counter)

    cv2.imshow('frame', frame)

    if cv2.waitKey(10) == 27:
        break

    if human_detecting_counter >= 10:
        ppid = os.getppid()
        os.kill(ppid,signal.SIGUSR1)
        print("signal send to",ppid)
        
        human_detecting_counter = 0
        time.sleep(10)

cv2.destroyAllWindows()