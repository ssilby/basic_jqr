#!/user/bin/env python3

import socket

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    host = socket.gethostname()
    port = 4444
    s.bind((host, port))

    while True:
        msg, addr = s.recvfrom(1024)
        print('message:', msg.decode('UTF-8'))
        print('from', addr)
        

if __name__ == "__main__":
    main()