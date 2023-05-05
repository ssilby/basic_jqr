#!/user/bin/env python3

import socket

def main():
    s = socket.socket()
    host = socket.gethostname()
    port = 4444

    s.connect((host, port))
    msg = s.recv(1024)
    print(msg.decode('UTF-8'))
    s.close()


if __name__ == "__main__":
    main()