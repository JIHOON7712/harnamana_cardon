import os
import signal
import time
import socket

import cv2


# 부모 프로세스의 PID를 가져옵니다.


#sleeping Detection 구현

# Variables to track blinking frequency
blink_counter = 0
blink_start_time = None

sleep_state = ""

# Load the pre-trained Haar cascade classifier for eye detection
eye_cascade = cv2.CascadeClassifier('haarcascade_eye.xml')

# Capture video feed from the webcam or external camera
cap = cv2.VideoCapture(0)


while True:
    # Read the current frame
    ret, frame = cap.read()
    
    # Convert the frame to grayscale for eye detection
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    cv2.line(frame, (160, 0), (160, 480), (255, 0, 0) ,2)
    cv2.line(frame, (480, 0), (480, 480), (255, 0, 0) ,2)
    cv2.line(frame, (0, 150), (800, 150), (255, 0, 0) ,2)
    cv2.line(frame, (0, 220), (800, 220), (255, 0, 0) ,2)

    # Detect eyes in the grayscale frame
    eyes = eye_cascade.detectMultiScale(gray, 1.3, 5)
    
    eyes_cnt = 0
    for (x, y, w, h) in eyes:
        # Draw rectangles around the detected eyes
        if 160 <= x and (x+w) <= 480 and 150 <= y and (y+h) <= 220:
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
            eyes_cnt += 1 
    
    # Measure the time duration between consecutive blinks

    if eyes_cnt == 0:
        if blink_start_time is None:
            blink_start_time = time.time()
        else:
            if time.time() - blink_start_time > 0.3:
                blink_counter += 1
                blink_start_time = None
    else:
        blink_start_time = None
    
    # In case of drowsiness, inform the driver.
    if blink_counter >= 10:
        print("졸음이 감지되었습니다!")
        blink_counter = 0
        sleep_state = "Sleepiness detection"

        # 시그널을 보냅니다.
        parent_pid = os.getppid()
        print("sleeping parent pid",parent_pid,"\n")
        os.kill(parent_pid, signal.SIGUSR1)

        # 10초 딜레이
        time.sleep(10)
    
    # Display the frame with eye rectangles and blinking frequency
    cv2.putText(frame, f"Blinks: {blink_counter}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
    cv2.putText(frame, f"{sleep_state}", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
    cv2.imshow('Drowsiness Detection', frame)
    
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