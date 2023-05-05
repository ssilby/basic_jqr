#!/usr/bin/env python3

from os import uname
import socket
import sys
import cmd
import socket
import struct
import hashlib


srvr_addr = ('localhost', 4444)

# try except on all receives


class Session(cmd.Cmd):
    intro = 'Welcome to the FTP Client. Type help or ? to list commands.\n'
    prompt = '(User) '
    socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    uname = None
    passwd = None
    priv = 0

    def do_login(self, arg):
        '''Login to the FTP Server'''
        self.socket.connect(srvr_addr)
        print("Connected to {:s}".format(repr(srvr_addr)))
        while(True):
            user_name = input("Enter your username:")
            password = input("Enter your password:").encode()
            hashed = hashlib.sha256(password)
            hashed = hashed.hexdigest()
            print(hashed)
            name_len = len(user_name)
            creds = struct.pack(f"64s{name_len}sB",
                                hashed.encode(), user_name.encode(), 0)
            print(hashed)

            self.socket.send(creds)
            data = self.socket.recv(1024)
            if len(data) == 0:
                print("server down")
                exit()
            data = int.from_bytes(data, 'little')
            print(data)
            if data == 1:
                print("Logged in as administrator")
                self.uname = user_name
                self.passwd = hashed
                self.priv = 1
                self.prompt = "(" + self.uname + ") "
                break
            elif data == 2:
                print("Logged in as user")
                self.uname = user_name
                self.passwd = hashed
                self.priv = 2
                self.prompt = "(" + self.uname + ") "
                break
            elif data == 3:
                print("Incorrect Password")
                continue
            elif data == 4:
                print("User not found")
                continue
            else:
                print("error")
                continue

    def do_get(self, args):
        if self.priv == 0:
            print("Please Login")
            return
        fname_size = len(args) + 1
        header = struct.pack("2i", 1, fname_size)
        print(args)

        # send get code and file name length
        self.socket.send(header)

        # send filename
        self.socket.send(args.encode())

        # receive packet count and size
        packetHeader = self.socket.recv(8)
        if len(packetHeader) == 0:
            print("server down")
            exit()
        packetCount, packetSize = struct.unpack('2i', packetHeader)
        print("packet count:", packetCount)

        # send back number of packets to ack
        self.socket.send(packetHeader)

        # receive data packetCount times and write to file
        with open(args, 'w') as f:
            count = 1
            for packet in range(packetCount):
                buffer = self.socket.recv(packetSize).decode('utf-8')
                if len(buffer) == 0:
                    print("server down")
                    exit()
                f.write(buffer)
                print("count is", count)
                ack_header = struct.pack("2i", count, 0)
                self.socket.send(ack_header)
                count += 1

    def do_put(self, args):
        if self.priv == 0:
            print("Please Login")
            return
        pass

    def do_dir(self, args):
        if self.priv == 0:
            print("Please Login")
            return

        # send dir request to server
        header = struct.pack("2i", 3, 0)
        self.socket.send(header)

        # receive number of files from server and send acknowledgement
        count = self.socket.recv(4)
        if len(count) == 0:
            print("server down")
            exit()
        count = int.from_bytes(count, 'little')
        header = struct.pack("2i", 7, count)
        self.socket.send(header)

        # receive and print 'count' number of times
        for packets in range(count):
            name_size = self.socket.recv(4)
            if len(name_size) == 0:
                print("server down")
                exit()
            name_size = int.from_bytes(name_size, 'little')
            filename = self.socket.recv(name_size)
            if len(filename) == 0:
                print("server down")
                exit()
            data = filename.decode('utf-8')
            print(data)

        return

    def do_add(self, args):
        if self.priv == 0:
            print("Please Login")
            return
        pass

    def do_del(self, args):
        if self.priv == 0:
            print("Please Login")
            return
        pass

    def do_quit(self, args):
        if self.socket:
            self.socket.close()
        return True


def main():

    sesh = Session()
    sesh.cmdloop()


if __name__ == '__main__':
    main()
