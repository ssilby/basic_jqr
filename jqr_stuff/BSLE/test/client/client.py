#!/usr/bin/env python3

from ctypes import sizeof
from os import uname
from os.path import isfile, isdir, join
import os
import socket
import sys
import cmd
import socket
import struct


class Session(cmd.Cmd):
    """
    The command module class that takes in all user commands

    ...

    Attributes
    ----------
    intro : str
        string that will be presented to user when program starts
    prompt: str
        the user prompt on the command line
        will present the username when logged in
    socket: socket object
        created when program starts up using port and ip provided command line
        used for all communication with server
    id: uint32_t
        unsigned int provided by server to be used as session id


    Methods
    -------
    connect(self, port, ip):

    do_login(self, arg):

    do_create_user(self, args):

    do_delete_user(self, args):

    do_get(self, args):

    do_put(self, args):

    do_delete(self, args):

    do_ls(self, args):

    do_mkdir(self, args):

    do_l_delete(self, args):

    do_l_ls(self, args):

    do_l_mkdir(self, args):

    do_quit(self, args):
    """

    intro = 'Welcome to the FTP Client. Type help or ? to list commands.\n'
    prompt = '(User) '
    try:
        socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print("Error creating socket: %s" % e)
        sys.exit(1)
    id = 0

    def connect(self, port, ip):
        '''
        Connects to remote server with port and ip address
        '''
        try:
            self.socket.connect((ip, int(port)))
            print("Connected to {:s}".format(repr(ip)))
        except socket.gaierror as e:
            print("Address-related error connecting to server: %s" % e)
            sys.exit(1)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

    def do_login(self, arg):
        '''
        login
        Login to the FTP Server
        Will be prompted for username followed by password
        '''

        while(True):
            user_name = input("Enter your username:").encode()
            password = input("Enter your password:").encode()

            opcode = 1
            user_flag = 0
            user_len = len(user_name)
            pass_len = len(password)

            if user_len == 0 or pass_len == 0:
                print("provide both username and password")
                return
            reserved = 0
            session_id = 0

            login_header = struct.pack(f"!2B3HI{user_len}s{pass_len}s", opcode, 
                                        user_flag, reserved, user_len, pass_len, 
                                        session_id, user_name, password)

            try:
                self.socket.send(login_header)
                data = self.socket.recv(6)
            except socket.error as e:
                print("Connection error: %s" % e)
                sys.exit(1)

            return_code, reserved, session_id = struct.unpack('>BBI', data)

            if (return_code == 1):
                self.prompt = "(" + user_name.decode() + ") "
                self.id = session_id
                print("login successful")
                print("opcode", return_code, "reserved",
                      reserved, "session_id:", self.id)
                break
            else:
                print("server action failed, try again")

    def do_create_user(self, args):
        '''
        create_user [username] [password] [permission level]
        Create a user with the following arguments after create user:
        username password permissions
        permissions are as follows:
        1: READ: can get files from server
        2: READ/WRITE: can get and put files from server
        3: ADMIN: can get and put files from server and delete existing users
        '''
        creds = args.split()
        if len(creds) != 3:
            print("create_user [username] [password] [permission level]")
            return

        username = creds[0].encode()
        password = creds[1].encode()

        opcode = 1
        user_flag = int(creds[2])
        if user_flag < 1 or user_flag > 3:
            print("requires permission between 1 and 3")
            return
            
        user_len = len(creds[0])
        pass_len = len(creds[1])
        reserved = 0
        session_id = self.id

        login_header = struct.pack(f"!2B3HI{user_len}s{pass_len}s", opcode,
                                   user_flag, reserved, user_len, pass_len, 
                                   session_id, username, password)

        try:
            self.socket.send(login_header)
            data = self.socket.recv(6)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

        return_code, reserved, session_id = struct.unpack('>BBI', data)

        parse_return(return_code)

    def do_delete_user(self, args):
        """
        delete_user [username]
        deletes user with [username] from server. Must have admin privileges to perform.
        """
        if not args:
            print("delete_user [username]")
            return

        username = args

        opcode = 1
        user_flag = 0xff
        user_len = len(username)
        pass_len = 0
        reserved = 0
        session_id = self.id

        login_header = struct.pack(f"!2B3HI{user_len}s", opcode, user_flag, 
                                    reserved, user_len, pass_len, session_id, 
                                    username.encode())

        try:
            self.socket.send(login_header)
            data = self.socket.recv(6)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

        return_code, reserved, session_id = struct.unpack('>BBI', data)

        parse_return(return_code)

    def do_get(self, args):
        '''
        get [src] [dst]
        Gets a file from server [src] path and copies it into the client [dst] path
        '''

        # user not allowed higher than client or server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        creds = args.split()

        if len(creds) != 2:
            print("get [src] [dst]")
            return

        src = creds[0]
        dst = creds[1]

        opcode = 0x04
        res = 0
        fname_len = len(src)
        req_header = struct.pack(
            f">2BHI{fname_len}s", opcode, res, fname_len, self.id, src.encode())

        try:
            # send get request
            self.socket.send(req_header)

            # receive response header minus content
            return_code = self.socket.recv(1)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

        return_code = int.from_bytes(return_code, "big")
        print(return_code)

        if return_code != 0x01:
            parse_return(return_code)
            return

        try:
            resp_header = self.socket.recv(5)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

        res, content_len = struct.unpack(">BI", resp_header)

        try:
            content = self.socket.recv(content_len).decode()
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

        with open(dst, 'w') as f:
            f.write(content)
            print("get success")

        return

    def do_put(self, args):
        '''
        puth [src] [dst]
        Sends a file from client [src] path to be placed in the server [dst] path
        '''

        # user not allowed higher than client or server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        response = input("Overwrite file if it exists? 'y' or 'n'")
        if response == "y":
            user_flag = 0x01
        elif response == "n":
            user_flag = 0x00
        else:
            print("invalid please type 'y' or 'n'")
            return

        creds = args.split()
        src = creds[0]
        dst = creds[1]

        file_size = os.path.getsize(src)
        if file_size > 1016:
            print("File is too large")
            return

        with open(src, 'r') as f:
            file_content = f.read()

        content_len = len(file_content)

        opcode = 0x06
        fname_len = len(dst)
        req_header = struct.pack(f">2BHII{fname_len}s{content_len}s", opcode, user_flag,
                                 fname_len, self.id, content_len, dst.encode(), file_content.encode())

        try:
            self.socket.send(req_header)
            # receive return code
            serv_response = self.socket.recv(6)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

        return_code, res, id = struct.unpack(">BBI", serv_response)

        parse_return(return_code)

    def do_delete(self, args):
        '''
        delete [path]
        Deletes file at server [path]
        '''

        # user not allowed higher than server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        if args[0] == '/':
            directory = args[1:]
        else:
            directory = args

        opcode = 2
        res = 0
        name_len = len(directory)

        del_header = struct.pack(
            f">2BHI{name_len}s", opcode, res, name_len, self.id, directory.encode())

        try:
            self.socket.send(del_header)
            response = self.socket.recv(6)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

        code, res, id = struct.unpack(">BBI", response)

        parse_return(code)

    def do_ls(self, args):
        '''
        ls [optional path]
        Lists remote directory contents
        '''

        # user not allowed higher than server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        if len(args) == 0:
            dir_len = 0
        else:
            print(args)
            dir_len = len(args)

        opcode = 3
        res = 0
        curr_position = 0

        total_content = bytearray()

        while(True):
            # send client request to server
            if dir_len == 0:
                ls_header = struct.pack(
                    f">2BHII", opcode, res, dir_len, self.id, curr_position)
            else:
                ls_header = struct.pack(
                    f">2BHII{dir_len}s", opcode, res, dir_len, self.id, curr_position, args.encode())

            try:
                self.socket.send(ls_header)
                ret_code = self.socket.recv(1)
            except socket.error as e:
                print("Connection error: %s" % e)
                sys.exit(1)

            ret_code = int.from_bytes(ret_code, "big")

            success = parse_return(ret_code)
            if success == 0:
                return

            try:
                server_response = self.socket.recv(15)
            except socket.error as e:
                print("Connection error: %s" % e)
                sys.exit(1)

            res1, res2, res3, total_len, content_len, curr_position = struct.unpack(
                ">3B3I", server_response)

            try:
                content = self.socket.recv(content_len)
            except socket.error as e:
                print("Connection error: %s" % e)
                sys.exit(1)

            total_content.extend(content)

            if curr_position == total_len:
                break

        files = total_content.split(b'\x00')
        for item in files:
            if item:
                item = item.decode()
                if item[0] == '1':
                    print("file:", item[1:])
                if item[0] == '2':
                    print("directory:", item[1:])

        return

    def do_mkdir(self, args):
        '''
        mkdir [path]
        Makes directory at server [path]
        '''

        # user not allowed higher than client or server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        dirname = args
        dirlen = len(args)

        opcode = 0x05
        res = 0
        reserved = 0
        req_header = struct.pack(
            f">2BH2I{dirlen}s", opcode, res, dirlen, self.id, reserved, dirname.encode())

        try:
            # send get request
            self.socket.send(req_header)

            # receive return code
            serv_response = self.socket.recv(6)
        except socket.error as e:
            print("Connection error: %s" % e)
            sys.exit(1)

        return_code, res, id = struct.unpack(">BBI", serv_response)

        parse_return(return_code)

    def do_l_delete(self, args):
        '''
        l_delete [path] 
        Deletes file at local [path]
        '''
        # user not allowed higher than client or server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        if os.path.exists(args):
            os.remove(args)
        else:
            print("File does not exist")

    def do_l_ls(self, args):
        '''
        l_ls [optional path] 
        Deletes file at local [path]
        '''

        if len(args) == 0:
            dir_list = os.listdir()
            files = [f for f in os.listdir(".") if isfile(join(".", f))]
            dirs = [d for d in os.listdir(".") if isdir(join(".", d))]
        else:
            # user not allowed higher than client or server home directory
            if ".." in args:
                print("no '..' allowed in path")
                return
            if os.path.exists(args):
                dir_list = os.listdir(args)
                files = [f for f in os.listdir(args) if isfile(join(args, f))]
                dirs = [d for d in os.listdir(args) if isdir(join(args, d))]
            else:
                print("File does not exist")
                return

        for directory in dirs:
            print("directory:", directory)
        for file in files:
            print("file:", file)

    def do_l_mkdir(self, args):
        '''
        l_mkdir [path]
        Makes directory at client [path]
        '''
        # user not allowed higher than client or server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        try:
            os.mkdir(args)
            print(f"{args} created")
        except FileExistsError:
            print(f"{args} already exists")

    def do_quit(self, args):
        '''
        quit
        Exits out of client
        '''
        if self.socket:
            self.socket.close()
        return True


def parse_return(return_code):
    """
    Helper function that reads return codes and prints action result
    """

    if return_code == 1:
        print("Server action was successful")
        return 1
    elif return_code == 2:
        print("Provided Session ID was invalid or expired")
        return 0
    elif return_code == 3:
        print("User associated with provided Session ID had insufficient permissions to perform the action")
        return 0
    elif return_code == 4:
        print("User could not be created because it already exists")
        return 0
    elif return_code == 5:
        print("File could not be created because it already exists")
        return 0
    elif return_code == 0xff:
        print("Server action failed")
        return 0
    else:
        print("Invalid return code")
        return 0


def main():

    if len(sys.argv) != 3:
        print("Please provide server port followed by server ipaddress")
        return -1

    # create instance of session command loop
    sesh = Session()
    # connect to server with provided credentials
    sesh.connect(sys.argv[1], sys.argv[2])
    # start command loop
    sesh.cmdloop()


if __name__ == '__main__':
    try:
        main()
    except (KeyboardInterrupt):
        print("Program Exiting")
