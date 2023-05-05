#!/user/bin/env python3

import socket

def main():
    msg = b'hey there'
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    host = socket.gethostname()
    port = 4444

    serverAddrPort = (host, port)

    s.sendto(msg, serverAddrPort)
    s.close()


if __name__ == "__main__":
    main()