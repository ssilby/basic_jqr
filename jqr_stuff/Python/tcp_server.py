#!/user/bin/env python3

import socket

def main():
    s = socket.socket()
    host = socket.gethostname()
    port = 4444
    s.bind((host, port))
    s.listen(5)

    while True:
        c, addr = s.accept()
        print('connected to:', addr)
        c.send(b'thank you for connecting to')
        c.close()    

if __name__ == "__main__":
    main()