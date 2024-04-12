import socket
import time

# 수신측 IP 주소와 포트 번호
receiver_ip = "192.168.1.11"
receiver_port = 50000

# 송신 데이터
message = "Hello from Raspberry Pi 1!"

# 송신 소켓 생성
sender_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while(1):
    sender_socket.sendto(message.encode(), (receiver_ip, receiver_port))
    time.sleep(2)

# 소켓 닫기
sender_socket.close()
