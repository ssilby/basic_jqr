'''
module docstring


Client Requirements


Written in Python and use network socks to communicate with a multi-threaded server.


Be able to log in with a username and password or create new user.


The password should use simple encryption to prevent passing of plaintext over a network (a simple XOR is sufficient for this)


Be able to join a lobby within the chat server


Be able to see a list of lobbies to join


Be able to see a list of users in a lobby


Be able to send messages to the lobby


Be able to private message another user


Be able to send a file to another user


Be able to receive a file from another user


Be able to deny or accept a file transfer request
'''

#!/usr/bin/env Python3

from email.headerregistry import HeaderRegistry
from os.path import isfile, isdir, join
import os
import socket
import cmd
import sys
import struct
import argparse
import hashlib
import threading
import time


class Session(cmd.Cmd):
    '''
    class docstring
    '''
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print("Error creating sock:", e)
        sys.exit(1)
    id = 0
    intro = 'Welcome to the Chat Client. Type help or ? to list commands.\n'
    prompt = '(User) '
    

    def connect(self, port, ip):
        '''
        connects to server on port and ip address
        '''
        try:
            self.sock.connect((ip, port))
            print("Connected to {:s}".format(repr(ip)))
        except socket.gaierror as sock_error:
            print("Address-related error connecting to server:", sock_error)
            return False
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return False
        return True


    def do_login(self, args):
        '''
        login
        Login to the FTP Server
        Will be prompted for username followed by password
        '''

        while True:
            user_name = input("Enter your username:").encode()
            password = input("Enter your password:").encode()

            try:
                hash_pass = hashlib.sha256(bytes(password)).hexdigest()
            except TypeError:
                print("password error")
                return
            
            opcode = 1
            user_flag = 0
            user_len = len(user_name)
            pass_len = len(hash_pass)

            if user_len == 0 or pass_len == 0:
                print("provide both username and password")
                return
            reserved = 0
            session_id = 0

            try:
                hash_pass = hash_pass.encode()
            except UnicodeEncodeError:
                print("error encoding password")
                return


            try:
                login_header = struct.pack(f"!2B3HI{user_len}s{pass_len}s", opcode, 
                                            user_flag, reserved, user_len, pass_len, 
                                            session_id, user_name, hash_pass)
            except struct.error as struct_error:
                print("error packing struct:", struct_error)
                return

            try:
                self.sock.send(login_header)
                data = self.sock.recv(6)
            except socket.error as sock_error:
                print("Connection error: ", sock_error)
                return

            try:
                return_code, reserved, session_id = struct.unpack('>BBI', data)
            except struct.error as struct_error:
                print("error unpacking struct:", struct_error)
                return

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
        if len(args) == 0:
            print("create_user [username] [password] [permission level]")
            return
        creds = args.split()
        if len(creds) != 3:
            print("create_user [username] [password] [permission level]")
            return

        username = creds[0].encode()
        password = creds[1].encode()

        try:
            hash_pass = hashlib.sha256(bytes(password)).hexdigest()
        except TypeError:
            print("password error")
            return

        opcode = 1
        user_flag = int(creds[2])
        if user_flag < 1 or user_flag > 3:
            print("requires permission between 1 and 3")
            return
            
        user_len = len(creds[0])
        pass_len = len(hash_pass)
        reserved = 0
        session_id = self.id

        try:
            hash_pass = hash_pass.encode()
        except UnicodeEncodeError:
            print("error encoding password")
            return

        try:
            login_header = struct.pack(f"!2B3HI{user_len}s{pass_len}s", opcode,
                                   user_flag, reserved, user_len, pass_len, 
                                   session_id, username, hash_pass)
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(login_header)
            data = self.sock.recv(6)
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return
        try:
            return_code, reserved, session_id = struct.unpack('>BBI', data)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

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

        try:
            login_header = struct.pack(f"!2B3HI{user_len}s", opcode, user_flag,
                                        reserved, user_len, pass_len, session_id,
                                        username.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(login_header)
            data = self.sock.recv(6)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return

        try:
            return_code, reserved, session_id = struct.unpack('>BBI', data)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

        parse_return(return_code)


    def do_create_lobby(self, args):
        """
        create_lobby [lobby name]
        creates a lobby with lobby name.
        """
        if len(args) == 0:
            print("create_lobby [lobby name]")
            return


        lname = args
        opcode = 0x07
        lobby_flag = 0x01
        lname_len = len(args)
        res = 0

        try:
            req_header = struct.pack(
                f">2B2HI{lname_len}s", opcode, lobby_flag, res, lname_len, self.id, lname.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
            data = self.sock.recv(6)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return

        try:
            return_code, reserved, session_id = struct.unpack('>BBI', data)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

        parse_return(return_code)


    def do_list_lobbies(self, args):
        '''
        list_lobbies
        Lists lobbies available to enter on chat server
        '''
        if self.id == 0:
            print("Not so fast! I see you Gourdine.. trying to list lobbies without logging in\n")
            return

        opcode = 0x07
        lobby_flag = 0x02
        res = 0
        namelen = 0

        try:
            req_header = struct.pack(
                ">2B2HI", opcode, lobby_flag, res, namelen, self.id)
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
            data = self.sock.recv(4)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return

        lobby_num = int.from_bytes(data, "big")

        print("number of lobbies", lobby_num)

        if lobby_num == 0:
            print("No lobbies on server")
            return
        else:
            for x in range(lobby_num):
                try:
                    data_len = self.sock.recv(4)
                except socket.error as sock_error:
                    print("Connection error: ", sock_error)
                    return
                data_len = int.from_bytes(data_len, "big")
                
                try:
                    data = self.sock.recv(data_len)
                except socket.error as sock_error:
                    print("Connection error: ", sock_error)
                    return

                data = data.decode()

                print(f"{x + 1}: {data}")

        return
    
    def do_enter(self, args):
        '''
        enter[lobby name]
        Enter a lobby in the chat server
        '''
        if len(args) == 0:
            print("create_lobby [lobby name]")
            return

        lname = args
        opcode = 0x07
        lobby_flag = 0x03
        lname_len = len(args)
        res = 0

        try:
            req_header = struct.pack(
                f">2B2HI{lname_len}s", opcode, lobby_flag, res, lname_len, self.id, lname.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
            data = self.sock.recv(6)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return

        try:
            return_code, reserved, session_id = struct.unpack('>BBI', data)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

        if return_code == 1:

            try:
                data = self.sock.recv(4)
            except socket.error as sock_error:
                print("Connection error: ", sock_error)
                return
            
            num = int.from_bytes(data, "big")
            print("entered lobby number", num)
            # thread to receive messages and print to screen
            try:
                thread = threading.Thread(target=thread_function, args=(self.sock,))
                thread.start()
            except (KeyboardInterrupt, SystemExit):
                print("Keyboard interrupt, quitting thread")
            lobby = Lobby(self.id, self.sock, lname, self.prompt, num)
            lobby.cmdloop()
            thread.join()
        else:
            print("failed to enter lobby")

        return


    def do_delete_lobby(self, args):
        '''
        delete_lobby [lobby name]
        deletes a lobby as long as there are no users in it
        '''
        if len(args) == 0:
            print("create_lobby [lobby name]")
            return

        lname = args
        opcode = 0x07
        lobby_flag = 0x04
        lname_len = len(args)
        res = 0

        try:
            req_header = struct.pack(
                f">2B2HI{lname_len}s", opcode, lobby_flag, res, lname_len, self.id, lname.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
            data = self.sock.recv(6)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return

        try:
            return_code, reserved, session_id = struct.unpack('>BBI', data)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

        parse_return(return_code)


    def do_get(self, args):
        '''
        get [src] [dst]
        Gets a file from server [src] path and copies it into the client [dst] path
        '''
        if len(args) == 0:
            print("get [src] [dst]")
            return
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

        try:
            req_header = struct.pack(
                f">2BHI{fname_len}s", opcode, res, fname_len, self.id, src.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            # send get request
            self.sock.send(req_header)

            # receive response header minus content
            return_code = self.sock.recv(1)
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return

        return_code = int.from_bytes(return_code, "big")

        print(return_code)

        if return_code != 0x01:
            parse_return(return_code)
            flush = self.sock.recv(5)
            return

        try:
            resp_header = self.sock.recv(5)
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return

        try:
            res, content_len = struct.unpack(">BI", resp_header)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

        try:
            content = self.sock.recv(content_len).decode()
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return

        try:
            with open(dst, 'w') as file:
                file.write(content)
                print("get success")
        except IOError:
            print("Provide valid destination filename")
            return

        return

    def do_put(self, args):
        '''
        puth [src] [dst]
        Sends a file from client [src] path to be placed in the server [dst] path
        '''
        if len(args) == 0:
            print("puth [src] [dst]")
            return
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

        try:
            file_size = os.path.getsize(src)
        except OSError as os_error:
            print("unable to find file:", os_error)
            return

        if file_size > 1016:
            print("File is too large")
            return

        with open(src, 'r') as file:
            file_content = file.read()
        
        content_len = len(file_content)

        # check if content_len is 0, if so, send without content

        opcode = 0x06
        fname_len = len(dst)

        if content_len != 0:
            file_content = file_content.encode()

        try:
            req_header = struct.pack(f">2BHII{fname_len}s{content_len}s", opcode, user_flag,
                                     fname_len, self.id, content_len, dst.encode(), file_content)
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
            # receive return code
            serv_response = self.sock.recv(6)
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return

        try:
            return_code, res, id = struct.unpack(">BBI", serv_response)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

        parse_return(return_code)


    def do_delete(self, args):
        '''
        delete [path]
        Deletes file at server [path]
        '''
        if len(args) == 0:
            print("delete [path]")
            return
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
        try:
            del_header = struct.pack(
                f">2BHI{name_len}s", opcode, res, name_len, self.id, directory.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(del_header)
            response = self.sock.recv(6)
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return

        try:
            code, res, id = struct.unpack(">BBI", response)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

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

        while True:
            # send client request to server
            if dir_len == 0:
                try:
                    ls_header = struct.pack(
                        ">2BHII", opcode, res, dir_len, self.id, curr_position)
                except struct.error as struct_error:
                    print("error packing struct:", struct_error)
                    return
            else:
                try:
                    ls_header = struct.pack(
                        f">2BHII{dir_len}s", opcode, res, dir_len, self.id, curr_position, args.encode())
                except struct.error as struct_error:
                    print("error packing struct:", struct_error)
                    return

            try:
                self.sock.send(ls_header)
                ret_code = self.sock.recv(1)
            except socket.error as sock_error:
                print("Connection error:", sock_error)
                return

            ret_code = int.from_bytes(ret_code, "big")

            success = parse_return(ret_code)
            if success == 0:
                return

            try:
                server_response = self.sock.recv(15)
            except socket.error as sock_error:
                print("Connection error:", sock_error)
                return
            try:
                res1, res2, res3, total_len, content_len, curr_position = struct.unpack(
                    ">3B3I", server_response)
            except struct.error as struct_error:
                print("error unpacking struct:", struct_error)
                return

            try:
                content = self.sock.recv(content_len)
            except socket.error as sock_error:
                print("Connection error:", sock_error)
                return

            total_content.extend(content)

            if curr_position == total_len:
                break

        files = total_content.split(b'\x00')
        if len(files) > 0:
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
        if len(args) == 0:
            print("mkdir [path]")
            return
        # user not allowed higher than client or server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        dirname = args
        dirlen = len(args)

        opcode = 0x05
        res = 0
        reserved = 0

        try:
            req_header = struct.pack(
                f">2BH2I{dirlen}s", opcode, res, dirlen, self.id, reserved, dirname.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            # send get request
            self.sock.send(req_header)

            # receive return code
            serv_response = self.sock.recv(6)
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return

        try:
            return_code, res, id = struct.unpack(">BBI", serv_response)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

        parse_return(return_code)

    def do_l_delete(self, args):
        '''
        l_delete [path]
        Deletes file at local [path]
        '''
        try:
            # user not allowed higher than client or server home directory
            if ".." in args:
                print("no '..' allowed in path")
                return

            if os.path.exists(args):
                os.remove(args)
            else:
                print("File does not exist")
        except OSError as del_error:
            print(del_error)

    def do_l_ls(self, args):
        '''
        l_ls [optional path] 
        Deletes file at local [path]
        '''

        if len(args) == 0:
            try:
                dir_list = os.listdir()
            except OSError as dir_error:
                print(dir_error)
                return
            
            files = [f for f in os.listdir(".") if isfile(join(".", f))]
            dirs = [d for d in os.listdir(".") if isdir(join(".", d))]
        else:
            # user not allowed higher than client or server home directory
            if ".." in args:
                print("no '..' allowed in path")
                return
            if os.path.isdir(args):
                dir_list = os.listdir(args)
                files = [f for f in os.listdir(args) if isfile(join(args, f))]
                dirs = [d for d in os.listdir(args) if isdir(join(args, d))]
            else:
                print("Directory does not exist")
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
        if len(args) == 0:
            print("l_mkdir [path]")
            return
        # user not allowed higher than client or server home directory
        if ".." in args:
            print("no '..' allowed in path")
            return

        try:
            os.mkdir(args)
            print(f"{args} created")
        except OSError as mkdir_error:
            print(mkdir_error)

    def do_quit(self, args):
        '''
        quit
        Exits out of client
        '''
        if self.sock:
            self.sock.close()
        return True


def parse_return(return_code):
    """
    Helper function that reads return codes and prints action result
    """

    if return_code == 1:
        print("Server action was successful")
        return 1
    elif return_code == 2:
        print("Provided Session id was invalid or expired")
        return 0
    elif return_code == 3:
        print("User associated with provided Session id had insufficient permissions to perform the action")
        return 0
    elif return_code == 4:
        print("User could not be created because it already exists")
        return 0
    elif return_code == 5:
        print("File could not be created because it already exists")
        return 0
    elif return_code == 6:
        print("Users in Lobby. Could not delete Lobby")
    elif return_code == 0xff:
        print("Server action failed")
        return 0
    else:
        print("Invalid return code")
        return 0


def thread_function(sock):
    '''
    receives chat messages and prints them to screen
    '''
    while True:
        try:
            # receive return code
            serv_response = sock.recv(12)
        except socket.error as sock_error:
            print("Connection error:", sock_error)
            return

        try:
            chat_flag, lobby_num, from_uname_len, to_uname_len, message_len, id = struct.unpack(">2B3HI", serv_response)
        except struct.error as struct_error:
            print("error unpacking struct:", struct_error)
            return

        if chat_flag == 0xff:
            print("user not in lobby")
        elif chat_flag == 0x03:
            print("exiting lobby")
            break
        elif chat_flag == 0x04:
            try:
                data = sock.recv(4)
            except socket.error as sock_error:
                print("Connection error: ", sock_error)
                return

            members_num = int.from_bytes(data, "big")

            print("number of members:", members_num)

            if members_num == 0:
                print("No lobbies on server")
            else:
                for x in range(members_num):
                    try:
                        data_len = sock.recv(4)
                    except socket.error as sock_error:
                        print("Connection error: ", sock_error)
                        return
                    name_len = int.from_bytes(data_len, "big")
                    print("name len:", name_len)
                    try:
                        data = sock.recv(name_len)
                    except socket.error as sock_error:
                        print("Connection error: ", sock_error)
                        return

                    data = data.decode()

                    print(f"{x + 1}: {data}")
        else:
            try:
                # receive return code
                uname = sock.recv(from_uname_len)
            except socket.error as sock_error:
                print("Connection error:", sock_error)
                return

            try:
                # receive return code
                msg = sock.recv(message_len)
            except socket.error as sock_error:
                print("Connection error:", sock_error)
                return
            
            uname = uname.decode('utf-8')
            msg = msg.decode('utf-8')
            print(f"({uname}) {msg}")

    return
    


class Lobby(cmd.Cmd):
    '''
    Loop for user to send and receive messages in a lobby
    '''
    def __init__(self, id, socket, lname, uname, num):
        self.id = id
        self.sock = socket
        self.lname = lname
        self.uname = uname
        self.lobby_num = num
        super().__init__()

    intro = "You are now in the lobby"
    prompt = ""

    def do_sendall(self, args):
        '''
        sendall [message]
        sends a message to everyone that is in the same lobby
        '''
        if len(args) == 0:
            print("create_lobby [lobby name]")
            return

        opcode = 0x08
        chat_flag = 0x01
        name = self.uname.replace("(","").replace(")","")
        from_uname_len = len(name)
        to_uname_len = 0
        message_len = len(args)
        message = args

        try:
            req_header = struct.pack(
                f">3B3HI{from_uname_len}s{message_len}s", opcode, chat_flag, self.lobby_num, from_uname_len, to_uname_len, message_len, self.id, name.encode(), message.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return

    
    def do_sendto(self, args):
        '''
        sendto [user name]
        message: [type message here]
        sends a message to a specific user in the lobby
        '''
        if len(args) == 0:
            print("create_lobby [lobby name]")
            return

        opcode = 0x08
        chat_flag = 0x02
        name = self.uname.replace("(","").replace(")","")
        uname_len = len(name) - 1
        to_name = args
        to_name_len = len(to_name)

        message = input("message:")
        message_len = len(message)

        if message_len == 0:
            print(" message: [type message here]")
            return

        try:
            req_header = struct.pack(
                f">3B3HI{uname_len}s{to_name_len}s{message_len}s", opcode, chat_flag, self.lobby_num,
                uname_len, to_name_len, message_len, self.id, name.encode(), to_name.encode(), message.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return


    def do_exit(self, args):
        '''
        exit
        exits the lobby
        '''
        opcode = 0x08
        chat_flag = 0x03

        name = self.uname.replace("(","").replace(")","")
        uname_len = len(name) - 1

        to_name_len = 0
        message_len = 0

        try:
            req_header = struct.pack(
                f">3B3HI{uname_len}s", opcode, chat_flag, self.lobby_num,
                uname_len, to_name_len, message_len, self.id, name.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return

        return True

    def do_list_members(self, args):
        '''
        list_members
        Lists memebers in current lobby
        '''
        opcode = 0x08
        chat_flag = 0x04
        name = self.uname.replace("(","").replace(")","")
        uname_len = len(name) - 1
        toname_len = 0
        message_len = 0

        try:
            req_header = struct.pack(
                f">3B3HI{uname_len}s", opcode, chat_flag, self.lobby_num,
                uname_len, toname_len, message_len, self.id, name.encode())
        except struct.error as struct_error:
            print("error packing struct:", struct_error)
            return

        try:
            self.sock.send(req_header)
        except socket.error as sock_error:
            print("Connection error: ", sock_error)
            return

    

def main():
    '''
    main function for client
    creates cmd loop session and connects to server
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument("Port", help="Chat Sever listening port", type=int)
    parser.add_argument("IP", help="IP Address or Hostname of Chat Server")
    args = parser.parse_args()

    # create instance of session command loop
    sesh = Session()
    # connect to server with provided credentials
    connected = sesh.connect(args.Port, args.IP)
    if connected is True:
        # start command loop
        sesh.cmdloop()
    else:
        print("unable to connect")



if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print("Program Exiting")
