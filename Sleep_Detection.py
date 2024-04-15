import os
import signal
import time
import socket

import cv2


# 부모 프로세스의 PID를 가져옵니다.


#sleeping Detection 구현

# 깜빡임 빈도를 추적하는 변수
blink_counter = 0   # 눈 감고 있을 때 카운트 변수
blink_start_time = None

sleep_state = ""

# 눈 감지를 위해 훈련된 haarcascade 로드
eye_cascade = cv2.CascadeClassifier('haarcascade_eye.xml')

# 카메라
cap = cv2.VideoCapture(0)

exit_flag = 0
start_cnt = 0
while True:
    # Read the current frame
    ret, frame = cap.read()
    
    # Convert the frame to grayscale for eye detection
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    cv2.line(frame, (160, 0), (160, 480), (255, 0, 0) ,2) # 영상처리 화면 내 구분자 설정
    cv2.line(frame, (480, 0), (480, 480), (255, 0, 0) ,2)
    cv2.line(frame, (0, 150), (800, 150), (255, 0, 0) ,2)
    cv2.line(frame, (0, 220), (800, 220), (255, 0, 0) ,2)

    start_cnt += 1
    if start_cnt >= 100: # py 실행 후 카운터가 100 이상일 때만 영상처리 실행(실행 후 바로 영상처리 되는걸 방지)
        # 그레이스케일 프레임에서 눈 감지
        eyes = eye_cascade.detectMultiScale(gray, 1.3, 5)
        
        eyes_cnt = 0
        for (x, y, w, h) in eyes:
            # 특정 범위에서만 눈을 감지
            if 160 <= x and (x+w) <= 480 and 150 <= y and (y+h) <= 220:
                cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
                eyes_cnt += 1  # 감지 시 카운터 증가
        
        # 연속적인 깜박임 사이의 시간 측정

        if eyes_cnt == 0:
            if blink_start_time is None:
                blink_start_time = time.time()
            else:
                if time.time() - blink_start_time > 0.3: # 0.3초 이상 눈이 검출 안될 시 깜빡임 카운터 변수 증가
                    blink_counter += 1
                    blink_start_time = None
        else:
            blink_start_time = None

        # 졸음 감지 시 영상 처리 종료
        if exit_flag == 1:
            time.sleep(2)
            break
        
        # In case of drowsiness, inform the driver.
        if blink_counter >= 10: # 깜빡임 카운터 변수 10 이상 시 졸음 판단
            print("졸음이 감지되었습니다!")
            blink_counter = 0
            sleep_state = "Sleepiness detection"
            exit_flag = 1

            # 시그널을 보냅니다.
            parent_pid = os.getppid()
            print("sleeping parent pid",parent_pid,"\n")
            os.kill(parent_pid, signal.SIGUSR1)

            # # 10초 딜레이
            # time.sleep(10)
    
    # Display the frame with eye rectangles and blinking frequency
    cv2.putText(frame, f"Blinks: {blink_counter}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    cv2.putText(frame, f"{sleep_state}", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
    # cv2.imshow('Drowsiness Detection', frame)
    
    # If 'q' is pushed, the loop will end.
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the video recording, then shut the window.
cap.release()
cv2.destroyAllWindows()

# 시그널을 보냅니다.
#parent_pid = os.getppid()
#print("sleeping parent pid",parent_pid,"\n")
#os.kill(parent_pid, signal.SIGUSR1)