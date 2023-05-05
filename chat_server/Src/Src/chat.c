/** @file server.c
 * 
 * @brief A server library to be used with a driver that spins off threads
 *
 * @par       
 * Steven Silbert, stevensilbertjr@gmail.com
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "../Includes/netlib.h"
#include "../Includes/file_lib.h"
#include "../Includes/pool_queue.h"
#include "../Includes/chat.h"
#include "../Includes/my_ht.h"


#define TABLE_SIZE 20
#define MAXDATASIZE 2048
#define SERVERPORT "4444"
#define THREAD_POOL_SIZE 20
#define PACKET_SIZE 1016
#define MAX_PATH 4096
#define LIST_SIZE 10

// global variable
extern volatile sig_atomic_t quit;




struct file_ 
{
    char *filename;
    FILE *fp;
    int fd;
};


char *
string_it(void *entry)
{
    user *new_entry = (user *) entry;
    return new_entry->uname;
}

void 
free_it(void *entry)
{
    user *ent = (user *) entry;
    printf("%s freed\n", ent->uname);
    free(ent);
    return;
}

struct hashtable
{
    void **entries;
    size_t size;
    size_t max_size;
    stringify string_it;
    free_h free_it;
};

struct user_request 
{
    uint8_t user_flag;
    uint16_t reserved;
    uint16_t uname_len;
    uint16_t pass_len;
    uint32_t session_id;
} __attribute__((__packed__));

struct lobby_request 
{
    uint8_t lobby_flag;
    uint16_t reserved;
    uint16_t lname_len;
    uint32_t session_id;
} __attribute__((__packed__));

struct chat_request
{
    uint8_t chat_flag;
    uint8_t lobby_num;
    uint16_t from_uname_len;
    uint16_t to_uname_len;
    uint16_t message_len;
    uint32_t session_id;
} __attribute__((__packed__));

struct serv_response 
{
    uint8_t return_code;
    uint8_t res; 
    uint32_t session_id;
} __attribute__((__packed__));

struct delete_f
{
    uint8_t reserved;
    uint16_t name_len;
    uint32_t session_id;
} __attribute__((__packed__));

struct ls_request
{
    uint8_t res;
    uint16_t name_len;
    uint32_t session_id;
    uint32_t current_pos;
    char dir_name[MAX_NAME];
} __attribute__((__packed__));

struct ls_response
{
    uint8_t opcode;
    uint8_t res1;
    uint8_t res2;
    uint8_t res3;
    uint32_t total_len;
    uint32_t message_len;
    uint32_t current_pos;
} __attribute__((__packed__));

struct get_request
{
    uint8_t res;
    uint16_t name_len;
    uint32_t session_id;
} __attribute__((__packed__));

struct get_response
{
    uint8_t ret_code;
    uint8_t res;
    uint32_t content_len;
} __attribute__((__packed__));

struct put_request
{
    uint8_t overwrite_flag;
    uint16_t name_len;
    uint32_t session_id;
    uint32_t content_len;
} __attribute__((__packed__));

struct mkdir_request
{
    uint8_t res;
    uint16_t dirname_len;
    uint32_t session_id;
    uint32_t reserved;
} __attribute__((__packed__));

// set global variable here for signal interrupt
void *
thread_function(void *arg)
{
    if (arg == NULL)
    {
        while (!quit) 
        {
            sleep(1);
            pthread_mutex_lock(&mutex);
            client_info *pclient = dequeue();
            pthread_mutex_unlock(&mutex);
            if (pclient != NULL) {
                // thread can do work
                handle_connection(pclient);
                free(pclient);
            }
        }
    }
    return NULL;
}

/*!
 * @brief Main code block for thread
 *
 * @param[in] p_client_socket  Pointer to struct containing client information
 *
 * @return void * to thread function
 */
void *
handle_connection(client_info *p_client_socket)
{
    while (!quit) 
    {
        if (p_client_socket->client_socket == -1) 
        {
            close(p_client_socket->client_socket);
            return NULL;
        }

        int rv = 0;
        uint8_t opcode = 0x00;

        // receive just the first byte of the request header to check operation type
        rv = recv(p_client_socket->client_socket, &opcode, sizeof(uint8_t), 0);
        if (rv == EINTR)
        {
            break;
        }
        if (rv == -1)
        {
            perror("recv");
            return NULL;
        }
        if (rv == 0)
        {
            fprintf(stderr, "connection closed\n");
            return NULL;
        }
        
        // User Operation
        if (opcode == 0x01)
        {
            user_operation(p_client_socket);
        }
        // Delete Remote File
        else if (opcode == 0x02)
        {
            delete_remote(p_client_socket);
        }
        // List Remote Directory
        else if (opcode == 0x03)
        {
            make_directory(p_client_socket);
        }
        // Put Remote File
        else if (opcode == 0x06)
        {
            put_file(p_client_socket);
        }
        else if (opcode == 0x07)
        {
            lobby_operation(p_client_socket);
        }
        else if (opcode == 0x08)
        {
            chat_operation(p_client_socket);
        }
        
    }
    shutdown(p_client_socket->client_socket, SHUT_RD);
    return NULL;
    
}


struct lobbies *
create_lobbies(void)
{
    struct lobbies *Lobbies = malloc(sizeof(*Lobbies));
    Lobbies->head = NULL;
    Lobbies->size = 0;

    return Lobbies;
}





/*!
 * @brief Populates hashtable with default admin user
 *
 * @param[in] none
 *
 * @return pointer to hash table
 */
hash_table *
get_creds(void)
{
     // create hash table
    hash_table *ht = create_ht(string_it, free_it, TABLE_SIZE);
    if (access("accounts.txt", F_OK) == 0) 
    {
        puts("file exists");
        // open file
        file_t *accounts = open_file("accounts.txt", "rb");
        int rv;
        // read struct from file
        while(1) 
        {
            user *entry = malloc(sizeof(*entry));
            if (!entry) 
            {
                close_file(accounts);
                free(entry);
                return NULL;
            }
            rv = read_file(accounts, entry, sizeof(*entry));
            // break when struct is null
            if (rv == 0) 
            {
                free(entry);
                break;
            }
            // add struct to hash table
            hash_table_insert(ht, entry);
        }
        
        // close file
        close_file(accounts);

    } 
    else 
    {
        user *user1 = calloc(1, sizeof(*user1));
        user *user2 = calloc(6 % 5, sizeof(*user2));
        // use memcpy here
        memcpy(user1->uname, "admin", 5);
        memcpy(&user1->passwd, "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8", 64);
        user1->perms = 3;
        memcpy(user2->uname, "phteven", 7);
        memcpy(&user2->passwd, "a1159e9df3670d549d04524532629f5477ceb7deec9b45e47e8c009506ecb2c8", 64);
        user2->perms = 2;

        printf("user1: uname: %s passwd: %s perms: %d\n", user1->uname, user1->passwd, user1->perms);
        printf("user2: uname: %s passwd: %s perms: %d\n", user2->uname, user2->passwd, user2->perms);
        hash_table_insert(ht, user1);
        hash_table_insert(ht, user2);

        file_t *accounts = open_file("accounts.txt", "wb");
        write_file(accounts, user1, sizeof(*user1));
        write_file(accounts, user2, sizeof(*user2));
        close_file(accounts);
    }

    return ht;

}


void
write_creds(hash_table *ht)
{
    file_t *accounts = open_file("accounts.txt", "wb");
    for (size_t i = 0; i < ht->max_size; ++i)
    {
        if (ht->entries[i] != NULL)
        {
            user *user1 = (user *) ht->entries[i];
            write_file(accounts, user1, sizeof(*user1));
        }
    }

    close_file(accounts);
}


/*!
 * @brief receives directory and sends contents of directory to client
 *
 * @param[in] p_client_socket pointer to client info struct
 *
 * @return Status code, 0 for success and negative for error code.
 */
int 
list_it(client_info *p_client_socket)
{
    int count, rv = 0;
    ls_request req;
    memset(&req, '\0', sizeof(req));
    
    if ((rv = recv(p_client_socket->client_socket, &req, sizeof(req), 0)) == -1) 
    {
        perror("recv");
        return -1;
    }
    
    req.session_id = ntohl(req.session_id);
    req.name_len = ntohs(req.name_len);
    req.current_pos = ntohl(req.current_pos);

    // file name too large
    if (req.name_len > MAX_NAME)
    {
        server_response(p_client_socket, 0xff);
        return -1;
    }

    uint8_t sesh_fail = 0x02;
    // make sure session id is valid
    if (req.session_id != p_client_socket->session_id)
    {
        send(p_client_socket->client_socket, &sesh_fail, sizeof(uint8_t), 0);
        return -1;
    }

    if (req.name_len > 0)
    {
        printf("Dir: %s\n", req.dir_name);
    }

    uint8_t fail = 0xff;
    DIR *d;
    struct dirent *dir;
    char *full_path = calloc(MAX_PATH, 1);
    if (!full_path)
    {
        fprintf(stderr, "not enough memory\n");
        return -1;
    }

    if (req.name_len == 0)
    {
        d = opendir(p_client_socket->dir);
        if (!d)
        {
            send(p_client_socket->client_socket, &fail, sizeof(uint8_t), 0);
            free(full_path);
            perror("opendir");
            return -1;
        }
    }
    else
    {
        memcpy(full_path, p_client_socket->dir, strlen(p_client_socket->dir));
        strncat(full_path, req.dir_name, strlen(req.dir_name));
        printf("full path: %s\n", full_path);
        d = opendir(full_path);
        if (!d)
        {
            send(p_client_socket->client_socket, &fail, sizeof(uint8_t), 0);
            perror("opendir");
            free(full_path);
            return -1;
        }
    }
    
    ls_response resp;
    memset(&resp, '\0', sizeof(resp));
    int content_len = 0;

    // get number of items in directory
    while ((dir = readdir(d)) != NULL)
    {
        content_len += snprintf(NULL, 0, "%s", dir->d_name);
        content_len += 2;
        count++;
    }

    closedir(d);
    // open directory again
    if (req.name_len == 0)
    {
        d = opendir(p_client_socket->dir);
        if (!d)
        {
            send(p_client_socket->client_socket, &fail, sizeof(uint8_t), 0);
            perror("opendir");
            free(full_path);
            return -1;
        }
    }
    else
    {
        d = opendir(full_path);
        if (!d)
        {
            send(p_client_socket->client_socket, &fail, sizeof(uint8_t), 0);
            perror("opendir");
            free(full_path);
            return -1;
        }
    }

    memset(&resp, '\0', sizeof(resp));
    char *content = calloc(content_len, 1);
    if (!content)
    {
        fprintf(stderr, "insufficient memory\n");
        free(full_path);
        return -1;
    }

    while ((dir = readdir(d)) != NULL) 
    {

        // get length of file dict entry and put in string buffer
        int dir_len = snprintf(NULL, 0, "%s", dir->d_name);
        

        // check if names are files or directories  
        if (dir->d_type == DT_REG) 
        {
            resp.total_len += snprintf(content + resp.total_len, dir_len + 2, "%d%s", 0x01, dir->d_name) + 1; 
        }
        else if (dir->d_type == DT_DIR)
        {
            resp.total_len += snprintf(content + resp.total_len, dir_len + 2, "%d%s", 0x02, dir->d_name) + 1; 
        }
        
    }
    closedir(d);
    
    int message_len = 0;
    if ((content_len - req.current_pos) < MAXDATASIZE)
    {
        message_len = (content_len - req.current_pos);
    }
    else
    {
        message_len = MAXDATASIZE;
    }
    resp.opcode = 0x01;
    resp.message_len = htonl(message_len);
    resp.current_pos = htonl(req.current_pos + message_len);
    resp.total_len = htonl(content_len);


    send(p_client_socket->client_socket, &resp, sizeof(resp), 0);
    send(p_client_socket->client_socket, content + req.current_pos, message_len, 0);
    free(full_path);
    free(content);

    return 0;
}


/*!
 * @brief receives filename and sends contents of file to client
 *
 * @param[in] p_client_socket pointer to client info struct
 *
 * @return Status code, 0 for success and negative for error code.
 */
int 
get_file(client_info *p_client_socket)
{
    int rv = 0;
    get_request req;
    memset(&req, '\0', sizeof(req));
    
    // receive request header
    if ((rv = recv(p_client_socket->client_socket, &req, sizeof(req), 0)) == -1) 
    {
        perror("recv");
        return -1;
    }
    puts("received");


    // make sure session id is valid
    if (ntohl(req.session_id) != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return -1;
    }

    uint16_t filename_len = ntohs(req.name_len);

    // file name too large
    if (filename_len > MAX_NAME)
    {
        server_response(p_client_socket, 0xff);
        return -1;
    }

    char *filename = calloc(filename_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, filename, filename_len, 0)) == -1) 
    {
        free(filename);
        perror("recv");
        return -1;
    }

    char *full_path = calloc(MAX_PATH, 1);
    if (!full_path)
    {
        fprintf(stderr, "not enough memory\n");
        return -1;
    }

    memcpy(full_path, p_client_socket->dir, strlen(p_client_socket->dir));
    strncat(full_path, filename, strlen(filename));


    // open file, send 0 if it doesn't exist
    file_t *file_info = open_file(full_path, "r");
    if (!file_info) 
    {
        free(filename);
        free(full_path);
        server_response(p_client_socket, 0xff);
        return -1;
    }
    puts("file opened");

    // divide file size by packet size to get number of packets
    fseek(file_info->fp, 0L, SEEK_END);
    uint32_t content_len = ftell(file_info->fp);
    rewind(file_info->fp);
    if (content_len > PACKET_SIZE) 
    {
        close_file(file_info);
        free(filename);
        free(full_path);
        server_response(p_client_socket, 0xff);
        return -1;
    } 


    
    // create buffer for contents of file to send
    char *buff = calloc(content_len, 1);
    if (!buff)
    {
        free(filename);
        free(full_path);
        fprintf(stderr, "insufficient memory\n");
        return -1;
    }

    rv = read_file(file_info, buff, content_len);
    if (!rv) 
    {
        close_file(file_info);
        free(filename);
        free(full_path);
        server_response(p_client_socket, 0xff);
        return -1;
    }

    get_response resp;
    memset(&resp, '\0', sizeof(resp));
    resp.ret_code = 0x01;
    resp.content_len = htonl(content_len);

    send(p_client_socket->client_socket, &resp, sizeof(resp), 0);
    send(p_client_socket->client_socket, buff, content_len, 0);

    close_file(file_info);
    free(full_path);
    free(filename);
    return 0;
}


/*!
 * @brief receives full path to file and writes contents to file
 *
 * @param[in] p_client_socket pointer to client info struct
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
put_file(client_info *p_client_socket)
{
    int rv = 0;
    put_request req;
    memset(&req, '\0', sizeof(req));
    
    // receive request header
    if ((rv = recv(p_client_socket->client_socket, &req, sizeof(req), 0)) == -1) 
    {
        perror("recv");
        return -1;
    }

    // make sure session id is valid
    if (ntohl(req.session_id) != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return -1;
    }

    // check permissions to write
    if (p_client_socket->perms < 2)
    {
        server_response(p_client_socket, 0x03);
        return -1;
    }

    uint16_t filename_len = ntohs(req.name_len);

    // file name too large
    if (filename_len > MAX_NAME)
    {
        server_response(p_client_socket, 0xff);
        return -1;
    }

    char *filename = calloc(filename_len + 1, 1);
    // receive the filename
    if ((rv = recv(p_client_socket->client_socket, filename, filename_len, 0)) == -1) 
    {
        free(filename);
        perror("recv");
        return -1;
    }

    char *full_path = calloc(MAX_PATH, 1);
    if (!full_path)
    {
        fprintf(stderr, "not enough memory\n");
        return -1;
    }

    memcpy(full_path, p_client_socket->dir, strlen(p_client_socket->dir));
    strncat(full_path, filename, strlen(filename));

    uint32_t content_len = ntohl(req.content_len);

    char *content = calloc(content_len, 1);
    if (!content)
    {
        free(filename);
        free(full_path);
        fprintf(stderr, "insufficient memory\n");
        return -1;
    }
    
    // receive the file contents
    if ((rv = recv(p_client_socket->client_socket, content, content_len, 0)) == -1) 
    {
        free(content);
        free(filename);
        free(full_path);
        perror("recv");
        return -1;
    }

    // try to open file, if it exists, check overwrite flag
    file_t *file_info = open_file(full_path, "r");
    if (file_info) 
    {
        if (req.overwrite_flag == 0x00)
        {
            free(filename);
            free(content);
            free(full_path);
            close_file(file_info);
            server_response(p_client_socket, 0x05);
            return -1;
        }
    }

    // create the file
    file_info = open_file(full_path, "w");

    if (!file_info)
    {
        server_response(p_client_socket, 0xff);
        free(filename);
        free(full_path);
        free(content);
        return -1;
    }

    // write contents to file and check for all bytes written
    uint32_t ret = write_file(file_info, content, content_len);

    if (ret != content_len)
    {
        close_file(file_info);
        server_response(p_client_socket, 0xff);
        free(filename);
        free(full_path);
        free(content);
        return -1;
    }
    
    server_response(p_client_socket, 1);

    close_file(file_info);
    free(filename);
    free(full_path);
    free(content);

    return 0;
}


/*!
 * @brief receives directory name and makes directory
 *
 * @param[in] p_client_socket pointer to client info struct
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
make_directory(client_info *p_client_socket)
{
    int rv = 0;
    mkdir_request req;
    memset(&req, '\0', sizeof(req));
    
    // receive request header
    if ((rv = recv(p_client_socket->client_socket, &req, sizeof(req), 0)) == -1) 
    {
        perror("recv");
        return -1;
    }

    // make sure session id is valid
    if (ntohl(req.session_id) != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return -1;
    }

    // check permissions to write
    if (p_client_socket->perms < 2)
    {
        server_response(p_client_socket, 0x03);
        return -1;
    }

    uint16_t dirname_len = ntohs(req.dirname_len);

    // file name too large
    if (dirname_len > MAX_NAME)
    {
        server_response(p_client_socket, 0xff);
        return -1;
    }

    char *dirname = calloc(dirname_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, dirname, dirname_len, 0)) == -1) 
    {
        free(dirname);
        perror("recv");
        return -1;
    }

    char *full_path = calloc(MAX_PATH, 1);
    if (!full_path)
    {
        fprintf(stderr, "not enough memory\n");
        return -1;
    }

    memcpy(full_path, p_client_socket->dir, strlen(p_client_socket->dir));
    strncat(full_path, dirname, strlen(dirname));


    DIR *dir = opendir(full_path);
    if (dir)
    {
        free(dirname);
        free(full_path);
        closedir(dir);
        server_response(p_client_socket, 0x05);
        return -1;
    }

    if ((rv = mkdir(full_path, 0777)) == -1)
    {
        free(dirname);
        free(full_path);
        perror("mkdir");
        server_response(p_client_socket, 0xff);
        return -1;
    }

    server_response(p_client_socket, 0x01);
    free(dirname);
    free(full_path);
    return 0;
    
}


/*!
 * @brief receives opcode for user and calls appropriate user operation
 *
 * @param[in] p_client_socket pointer to client info struct
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
user_operation(client_info *p_client_socket)
{
    user_request *req = calloc(sizeof(*req), 1);
    
    int ret = 0;

    while(!quit)
    {
        // receive the rest of the user request header
        int rv = 0;
        if ((rv = recv(p_client_socket->client_socket, req, sizeof(*req), 0)) == -1) 
        {
            free(req);
            perror("recv");
            return rv;
        }
        
        // change multibyte ints to host endianness
        req->uname_len = ntohs(req->uname_len);
        req->pass_len = ntohs(req->pass_len);
        req->session_id = ntohl(req->session_id);
        

        // user login, no session id req'd
        if (req->user_flag == 0x00)
        {
            ret = user_login(p_client_socket, req);
            free(req);
            return ret;
        }
        // create read only user (permission level 1)
        else if (req->user_flag == 0x01)
        {
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            ret = create_user(p_client_socket, req, 1);
            free(req);
            return ret;
        }
        // create read/write user (permission level 2)
        else if (req->user_flag == 0x02)
        {
            // check permission level
            if (p_client_socket->perms < 2)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            ret = create_user(p_client_socket, req, 2);
            free(req);
            return ret;
        }
        // create admin user (permission level 3)
        else if (req->user_flag == 0x03)
        {
            // check permission level
            if (p_client_socket->perms < 3)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            ret = create_user(p_client_socket, req, 3);
            free(req);
            return ret;
        }
        // delete user
        else if (req->user_flag == 0xff)
        {
            ret = delete_user(p_client_socket, req);
            free(req);
            return ret;
        }
        else
        {
            // invalid flag
            server_response(p_client_socket, 0xff);
            free(req);
            return -1;
        }
    }
    free(req);
    return ret;
}


/*!
 * @brief checks username and password. Sends session id to client on success
 *
 * @param[in] p_client_socket pointer to client info struct
 * @param[in] req pointer to struct containing user request header
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
user_login(client_info *p_client_socket, user_request *req)
{
    // receive the rest of the packet containing username and password
    int rv = 0;
    char *username = calloc(req->uname_len + 1, 1);
    char *password = calloc(req->pass_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, username, req->uname_len, 0)) == -1) 
    {
        free(username);
        free(password);
        perror("recv");
        return rv;
    }


    if ((rv = recv(p_client_socket->client_socket, password, req->pass_len, 0)) == -1) 
    {
        free(username);
        free(password);
        perror("recv");
        return rv;
    }


    // look up user in the hashtable
    user *new_user = (user *) hash_table_lookup(p_client_socket->ht, username); 

    // send error if no user exists
    if (!new_user)
    {
        free(username);
        free(password);
        fprintf(stderr, "unable to locate user\n");
        server_response(p_client_socket, 0xff);
        return -1;
    }

    // send success response with session id if password matches
    if (!strncmp(password, new_user->passwd, strlen(new_user->passwd)))
    {
        // get permission level in main struct to check further actions
        p_client_socket->perms = new_user->perms;
        
        serv_response response;
        response.return_code = 0x01;
        response.res = 0;
        response.session_id = htonl(p_client_socket->session_id);
        memcpy(p_client_socket->uname, username, req->uname_len);

        send(p_client_socket->client_socket, &response, sizeof(response), 0);
        free(username);
        free(password);
        return 0;
    }
    // send error is password doesn't match
    else
    {
        free(username);
        free(password);
        fprintf(stderr, "incorrect password\n");
        server_response(p_client_socket, 0xff);
        return -1;
    }


    free(username);
    free(password);
    
    return 0;   
        
}


/*!
 * @brief creates a user and inserts into hash table
 *
 * @param[in] p_client_socket pointer to client info struct
 * @param[in] req pointer to struct containing user request header
 * @param[in] permission int showing permission level
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
create_user(client_info *p_client_socket, user_request *req, const int permission)
{
    // make sure session id is valid
    if (req->session_id != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return -1;
    }
    // receive the rest of the packet containing username and password
    int rv = 0;
    char *username = calloc(req->uname_len + 1, 1);
    char *password = calloc(req->pass_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, username, req->uname_len, 0)) == -1) 
    {
        perror("recv");
        free(username);
        free(password);
        return -1;
    }

    if ((rv = recv(p_client_socket->client_socket, password, req->pass_len, 0)) == -1) 
    {
        free(username);
        free(password);
        perror("recv");
        return -1;
    }

    user *new_user = (user *) hash_table_lookup(p_client_socket->ht, username);
    if (!new_user)
    {
        user *user = calloc(sizeof(*user), 1);
        memcpy(&user->uname, username, strlen(username));
        memcpy(&user->passwd, password, strlen(password));
        user->perms = permission;
        int ret = hash_table_insert(p_client_socket->ht, user);
        if (ret == 0)
        {
            free(username);
            free(password);
            server_response(p_client_socket, 0x01);
            return 0;
        }
        else
        {
            server_response(p_client_socket, 0xff);
            free(username);
            free(password);
            free(user);
            return -1;
        }
    }
    else{
        server_response(p_client_socket, 0x04);
        free(username);
        free(password);
        return -1;
    }
}


/*!
 * @brief deletes user from table using username
 *
 * @param[in] p_client_socket pointer to client info struct
 * @param[in] req pointer to struct containing user request header
 *
 * @return Status code, 0 for success and negative for error code.
 */
int delete_user(client_info *p_client_socket, user_request *req)
{
    // make sure session id is valid
    if (req->session_id != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return -1;
    }
    // only admins can delete users
    if (p_client_socket->perms < 3)
    {
        server_response(p_client_socket, 0x03);
        return -1;
    }
    // receive the rest of the packet containing username and password
    int rv = 0;
    char *username = calloc(req->uname_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, username, req->uname_len, 0)) == -1) 
    {
        perror("recv");
        free(username);
        return -1;
    }

    user *new_user = (user *) hash_table_lookup(p_client_socket->ht, username);
    if (!new_user)
    {
        free(username);
        server_response(p_client_socket, 0xff);
        return -1;
    }
    else
    {
        rv = remove_entry(p_client_socket->ht, username);
        server_response(p_client_socket, 0x01);
    }

    free(username);
    return 0;
    
}


/*!
 * @brief deletes file from server
 *
 * @param[in] p_client_socket pointer to client info struct
 *
 * @return Status code, 0 for success and negative for error code.
 */
int delete_remote(client_info *p_client_socket)
{
    // check permissions
    if (p_client_socket->perms < 3)
    {
        server_response(p_client_socket, 0x03);
    }
    
    //receive rest of header
    delete_f delete_header;
    int rv = 0;
    if ((rv = recv(p_client_socket->client_socket, &delete_header, sizeof(delete_header), 0)) == -1) 
    {
        perror("recv");
        return rv;
    }
    
    delete_header.name_len = ntohs(delete_header.name_len);
    // file name too large
    if (delete_header.name_len > MAX_NAME)
    {
        server_response(p_client_socket, 0xff);
        return -1;
    }
    // make sure session id is valid
    if (p_client_socket->session_id != htonl(delete_header.session_id))
    {
        server_response(p_client_socket, 0x02);
        return -1;
    }

    // get filename
    char *file_name = calloc(delete_header.name_len + 1, 1);
    if ((rv = recv(p_client_socket->client_socket, file_name, delete_header.name_len, 0)) == -1) 
    {
        perror("recv");
        return -1;
    }  

    //add file path to directory
    char *directory = calloc(strlen(p_client_socket->dir) + strlen(file_name) + 1, 1);
    if (!directory)
    {
        free(file_name);
        server_response(p_client_socket, 0xff);
        return -1;
    }

    memcpy(directory, p_client_socket->dir, strlen(p_client_socket->dir));

    strncat(directory, file_name, strlen(file_name));

    if (remove(directory) == 0)
    {  
        server_response(p_client_socket, 0x01);
    } 
    else
    {
        server_response(p_client_socket, 0xff);
        free(directory);
        free(file_name);
        return -1;
    }
    free(file_name);
    return 0;
}

/*!
 * @brief sends response back to client
 *
 * @param[in] p_client_socket pointer to client info struct
 * @param[in] code return code
 *
 * @return Status code, 0 for success and negative for error code.
 */
void
server_response(client_info *p_client_socket, uint8_t code)
{
    serv_response response;
    response.return_code = code;
    response.res = 0;
    response.session_id = 0;
    send(p_client_socket->client_socket, &response, sizeof(response), 0);
    return;
}


/*!
 * @brief receives opcode for user and calls appropriate lobby operation
 *
 * @param[in] p_client_socket pointer to client info struct
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
lobby_operation(client_info *p_client_socket)
{
    lobby_request *req = calloc(sizeof(*req), 1);
    
    
    int ret = 0;

    while(!quit)
    {
        // receive the rest of the user request header
        int rv = 0;
        if ((rv = recv(p_client_socket->client_socket, req, sizeof(*req), 0)) == -1) 
        {
            free(req);
            perror("recv");
            return rv;
        }
        
        // change multibyte ints to host endianness
        req->lname_len = ntohs(req->lname_len);
        req->session_id = ntohl(req->session_id);
        

        // user login, no session id req'd
        if (req->lobby_flag == 0x01)
        {
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }

            ret = create_lobby(p_client_socket, req);
            free(req);
            return ret;
        }

        // list current lobbies in server
        else if (req->lobby_flag == 0x02)
        {
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            list_lobbies(p_client_socket, req);
            free(req);
            return ret;
        }
        
        // add user to lobby
        else if (req->lobby_flag == 0x03)
        {
            // check permission level
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            enter_lobby(p_client_socket, req);
            free(req);
            return 0;
        }
        // destroy lobby
        else if (req->lobby_flag == 0x04)
        {
            // check permission level
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            destroy_lobby(p_client_socket, req);
            free(req);
            return 0;
        }
        else
        {
            // invalid flag
            server_response(p_client_socket, 0xff);
            free(req);
            return -1;
        }
    }
    free(req);
    return ret;
}



int
create_lobby(client_info *p_client_socket, lobby_request *req)
{
    // make sure session id is valid
    if (req->session_id != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return -1;
    }
    // receive the rest of the packet containing username and password
    int rv = 0;

    struct lobby *new_lobby = calloc(sizeof(*new_lobby), 1);
    new_lobby->name_len = req->lname_len;
    new_lobby->lobby_num = p_client_socket->lobs->size + 1;
    new_lobby->lobby_size = 0;
    new_lobby->clients = calloc(sizeof(struct client_list), 1);
    new_lobby->lobby_name = calloc(req->lname_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, new_lobby->lobby_name, req->lname_len, 0)) == -1) 
    {
        perror("recv");
        free(new_lobby->lobby_name);
        free(new_lobby);
        return -1;
    }

    if (p_client_socket->lobs->head == NULL)
    {
        p_client_socket->lobs->head = new_lobby;
    }
    else
    {
        struct lobby *curr = p_client_socket->lobs->head;
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = new_lobby;
    }

    p_client_socket->lobs->size++;
    server_response(p_client_socket, 0x01);

    return 0;
}


void
list_lobbies(client_info *p_client_socket, lobby_request *req)
{
    // make sure session id is valid
    if (req->session_id != p_client_socket->session_id)
    {
        int fail = 666;
        fail = htonl(fail);
        send(p_client_socket->client_socket, &fail, sizeof(int), 0);
        return;
    }

    // only admins can delete users
    if (p_client_socket->perms < 1)
    {
        int fail = 666;
        fail = htonl(fail);
        send(p_client_socket->client_socket, &fail, sizeof(int), 0);
        return;
    }

    printf("number of lobbies %d\n", p_client_socket->lobs->size);
    // get number of lobbies and send to client
    int lobby_num = p_client_socket->lobs->size;
    int lobby_num_net = htonl(lobby_num);
    send(p_client_socket->client_socket, &lobby_num_net, sizeof(int), 0);

    if (lobby_num != 0)
    {
        // iterate through linked list of lobbies and send each name to client
        struct lobby *l = p_client_socket->lobs->head;
        printf("lobby head: %s\n", l->lobby_name);
        while (l != NULL)
        {
            int name_len = htonl(l->name_len);
            send(p_client_socket->client_socket, &name_len, sizeof(int), 0);
            printf("name len: %d\n", l->name_len);
            send(p_client_socket->client_socket, l->lobby_name, l->name_len, 0);
            l = l->next;
        }
    }
}


void
enter_lobby(client_info *p_client_socket, lobby_request *req)
{
     // make sure session id is valid
    if (req->session_id != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return;
    }

    int rv;

    char *lobby_name = calloc(req->lname_len + 1, 1);
    struct client *new_client = calloc(sizeof(*new_client), 1);

    if ((rv = recv(p_client_socket->client_socket, lobby_name, req->lname_len, 0)) == -1) 
    {
        perror("recv");
        free(lobby_name);
        return;
    }

    new_client->name = calloc(sizeof(new_client->name) + 1, 1);
    memcpy(new_client->name, p_client_socket->uname, strlen(p_client_socket->uname));

    printf("%s has entered %s\n", new_client->name, lobby_name);
    new_client->name_len = strlen(new_client->name);
    new_client->fd = p_client_socket->client_socket;
    new_client->next = NULL;

    // create user struct
    // add user to lobby
    // if no head set to head, otherwise iterate through list
    // ++ lobby size

    // find lobby by name
    struct lobby *l = p_client_socket->lobs->head;
    while (l != NULL)
    {
        if (!strncmp(lobby_name, l->lobby_name, req->lname_len))
        {
            new_client->lobby_num = l->lobby_num;
            if (l->clients->head == NULL)
            {
                l->clients->head = new_client;
                l->lobby_size++;
                free(lobby_name);
                server_response(p_client_socket, 0x01);
                int num = htonl(new_client->lobby_num);
                send(p_client_socket->client_socket, &num, sizeof(int), 0);
                printf("Entered lobby number %d\n", new_client->lobby_num);
                return;
            }
            else
            {
                struct client *c = l->clients->head;
                while (c->next != NULL)
                {
                    c = c->next;
                }
                c->next = new_client;
                l->lobby_size++;
                free(lobby_name);
                server_response(p_client_socket, 0x01);
                int num = htonl(new_client->lobby_num);
                send(p_client_socket->client_socket, &num, sizeof(int), 0);
                printf("Entered lobby number %d\n", new_client->lobby_num);
                return;
            }
            break;
        }
        else
        {
            l = l->next;
        }

    }
    server_response(p_client_socket, 0x0ff);

    

    free(lobby_name);
    return;
}


void
destroy_lobby(client_info *p_client_socket, lobby_request *req)
{
    int rv;
    char *lobby_name = calloc(req->lname_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, lobby_name, req->lname_len, 0)) == -1) 
    {
        perror("recv");
        free(lobby_name);
        return;
    }

    struct lobby *l = p_client_socket->lobs->head;
    // find the lobby to be deleted
    if (l->name_len == req->lname_len && !strncmp(lobby_name, l->lobby_name, req->lname_len))
    {   
        // return error if lobby is not empty
        if (l->lobby_size > 0)
        {
            server_response(p_client_socket, 0x06);
            free(lobby_name);
            return;
        }
        p_client_socket->lobs->head = l->next;
        p_client_socket->lobs->size--;
        free(l->clients);
        free(l);
        server_response(p_client_socket, 0x01);
    }
    else
    {
        while (l->next != NULL)
        {
            if (l->next->name_len == req->lname_len && !strncmp(lobby_name, l->next->lobby_name, req->lname_len))
            {
                // return error if lobby is not empty
                if (l->lobby_size > 0)
                    {
                        server_response(p_client_socket, 0x06);
                        free(lobby_name);
                        return;
                    }
                struct lobby *tmp = l->next;
                l->next = l->next->next;
                p_client_socket->lobs->size--;
                free(tmp->clients);
                free(tmp);
                server_response(p_client_socket, 0x01);
                break;
            }
            l = l->next;
        }
    }
    
    free(lobby_name);
    return;
}


void
destroy_lobbies(struct lobbies *l)
{
    struct lobby *current = l->head;
    struct lobby *next;

    while (current != NULL)
    {
        next = current->next;
        if (current->lobby_size > 0)
        {
            struct client *curr = current->clients->head;
            struct client *nxt;
            while (curr != NULL)
            {
                nxt = curr->next;
                free(curr->name);
                free(curr);
                curr = nxt;
            }
        }
        free(current->clients);
        free(current->lobby_name);
        free(current);
        current = next;
    }

    return;
}


/*!
 * @brief receives opcode for user and calls appropriate chat operation
 *
 * @param[in] p_client_socket pointer to client info struct
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
chat_operation(client_info *p_client_socket)
{
    chat_request *req = calloc(sizeof(*req), 1);
    
    int ret = 0;

    while(!quit)
    {
        // receive the rest of the user request header
        int rv = 0;
        if ((rv = recv(p_client_socket->client_socket, req, sizeof(*req), 0)) == -1) 
        {
            free(req);
            perror("recv");
            return rv;
        }
        
        // change multibyte ints to host endianness
        req->from_uname_len = ntohs(req->from_uname_len);
        req->to_uname_len = ntohs(req->to_uname_len);
        req->message_len = ntohs(req->message_len);
        req->session_id = ntohl(req->session_id);
        
        
        // send message to all users
        if (req->chat_flag == 0x01)
        {
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }

            send_all(p_client_socket, req);
            free(req);
            return ret;
        }
        
        // send message to specific user by name
        else if (req->chat_flag == 0x02)
        {
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            send_direct(p_client_socket, req);
            free(req);
            return ret;
        }
        
        // exit lobby
        else if (req->chat_flag == 0x03)
        {
            // check permission level
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            exit_room(p_client_socket, req);
            free(req);
            return 0;
        }
        // list members
        else if (req->chat_flag == 0x04)
        {
            // check permission level
            if (p_client_socket->perms < 1)
            {
                server_response(p_client_socket, 0x03);
                free(req);
                return -1;
            }
            list_members(p_client_socket, req);
            free(req);
            return 0;
        }
        else
        {
            // invalid flag
            server_response(p_client_socket, 0xff);
            free(req);
            return -1;
        }
    }
    free(req);
    return ret;
}


void
send_all(client_info *p_client_socket, chat_request *req)
{
     // make sure session id is valid
    if (req->session_id != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return;
    }

    int rv;

    char *uname = calloc(req->from_uname_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, uname, req->from_uname_len, 0)) == -1) 
    {
        perror("recv");
        free(uname);
        return;
    }

    char *msg = calloc(req->message_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, msg, req->message_len, 0)) == -1) 
    {
        perror("recv");
        free(msg);
        free(uname);
        return;
    }

    printf("message: %s\n", msg);

    req->from_uname_len = htons(req->from_uname_len);
    req->message_len = htons(req->message_len);
    req->session_id = htonl(req->session_id);

    // find lobby by number
    struct lobby *l = p_client_socket->lobs->head;
    while (l != NULL)
    {
        if (req->lobby_num == l->lobby_num)
        {
            // check if sender is the only user in lobby
            if (l->lobby_size == 1)
            {
                printf("only one person in lobby\n");
                break;
            }
            struct client *c = l->clients->head;
            while (c != NULL)
            {
                // don't send message to self
                if (!strncmp(uname, c->name, req->from_uname_len))
                {
                    printf("skipping self\n");
                    c = c->next;
                    continue;
                }
                printf("sending to %s\n", uname);
                send(c->fd, req, sizeof(*req), 0);
                send(c->fd, uname, strlen(uname), 0);
                send(c->fd, msg, strlen(msg), 0);
                c = c->next;
            }
            break;
        }
        l = l->next;
    }
    free(msg);
    free(uname);
}


void
send_direct(client_info *p_client_socket, chat_request *req)
{
    // make sure session id is valid
    if (req->session_id != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return;
    }

    int rv;

    char *f_uname = calloc(req->from_uname_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, f_uname, req->from_uname_len, 0)) == -1) 
    {
        perror("recv");
        free(f_uname);
        return;
    }

    char *to_name = calloc(req->to_uname_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, to_name, req->to_uname_len, 0)) == -1) 
    {
        perror("recv");
        free(to_name);
        free(f_uname);
        return;
    }

    char *msg = calloc(req->message_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, msg, req->message_len, 0)) == -1) 
    {
        perror("recv");
        free(to_name);
        free(f_uname);
        free(msg);
        return;
    }

    printf("message: %s\n", msg);
    
    req->message_len = htons(req->message_len);
    req->session_id = htonl(req->session_id);

    // find lobby by number
    struct lobby *l = p_client_socket->lobs->head;
    while (l != NULL)
    {
        if (req->lobby_num == l->lobby_num)
        {
            // check if sender is the only user in lobby
            if (l->lobby_size == 1)
            {
                printf("only one person in lobby\n");
                break;
            }
            struct client *c = l->clients->head;
            while (c != NULL)
            {
                if (c->name_len == req->to_uname_len)
                {
                    // find user to send direct message to
                    if (!strncmp(to_name, c->name, req->to_uname_len))
                    {
                        printf("req size: %lu\n", sizeof(*req));
                        req->from_uname_len = htons(req->from_uname_len);
                        send(c->fd, req, sizeof(*req), 0);
                        printf("sending to %s\n", c->name);
                        send(c->fd, f_uname, strlen(f_uname), 0);
                        send(c->fd, msg, strlen(msg), 0);
                        free(msg);
                        free(f_uname);
                        free(to_name);
                        return;
                    }
                }
                c = c->next;
            }
            c = l->clients->head;
            while (c != NULL)
            {
                // find sender to send error message to
                if (!strncmp(f_uname, c->name, req->from_uname_len))
                {
                    req->from_uname_len = htons(req->from_uname_len);
                    puts("invalid username");
                    req->chat_flag = 0xff;
                    send(c->fd, req, sizeof(*req), 0);
                    break;
                }
                c = c->next;
            }
            break;
        }
        l = l->next;
    }
    free(msg);
    free(f_uname);
    free(to_name);
}


void
exit_room(client_info *p_client_socket, chat_request *req)
{
    char *f_uname = calloc(req->from_uname_len + 1, 1);
    int rv;

    if ((rv = recv(p_client_socket->client_socket, f_uname, req->from_uname_len, 0)) == -1) 
    {
        perror("recv");
        free(f_uname);
        return;
    }

    // find user in library
    struct lobby *l = p_client_socket->lobs->head;

    while (l != NULL)
    {
        if (req->lobby_num == l->lobby_num)
        {
            struct client *c = l->clients->head;

            if (c->name_len == req->from_uname_len && !strncmp(f_uname, c->name, req->from_uname_len))
            {
                l->clients->head = c->next;
                send(c->fd, req, sizeof(*req), 0);
                free(c->name);
                free(c);
            }
            else
            {
                while (c != NULL)
                {
                    if (c->next->name_len == req->from_uname_len && !strncmp(f_uname, c->next->name, req->from_uname_len))
                    {
                        struct client *tmp = c->next;
                        c->next = c->next->next;
                        send(tmp->fd, req, sizeof(*req), 0);
                        free(tmp->name);
                        free(tmp);
                        break;
                    }
                    c = c->next;
                }
            }
            l->lobby_size--;
            break;
        }
        l = l->next;
    }
    free(f_uname);

}


void
list_members(client_info *p_client_socket, chat_request *req)
{
    // make sure session id is valid
    if (req->session_id != p_client_socket->session_id)
    {
        server_response(p_client_socket, 0x02);
        return;
    }

    // get number of members and send to client
    struct lobby *l = p_client_socket->lobs->head;

    while(l != NULL)
    {
        if (l->lobby_num == req->lobby_num)
        {
            send(p_client_socket->client_socket, req, sizeof(*req), 0);
            int members_num = l->lobby_size;
            int members_num_net = htonl(members_num);
            send(p_client_socket->client_socket, &members_num_net, sizeof(int), 0);
            struct client *c = l->clients->head;
            while (c != NULL)
            {
                int name_len = htonl(c->name_len);
                send(p_client_socket->client_socket, &name_len, sizeof(int), 0);
                send(p_client_socket->client_socket, c->name, c->name_len, 0);
                c = c->next;
            }
            break;
        }
        l = l->next;
    }
}

