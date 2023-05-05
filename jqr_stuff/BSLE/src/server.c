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

#include "netlib.h"
#include "file_lib.h"
#include "pool_queue.h"
#include "server.h"


#define TABLE_SIZE 20
#define MAXDATASIZE 2048
#define SERVERPORT "4444"
#define THREAD_POOL_SIZE 20
#define PACKET_SIZE 1016
#define MAX_NAME 100
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
    return;
}

struct user_request 
{
    uint8_t user_flag;
    uint16_t reserved;
    uint16_t uname_len;
    uint16_t pass_len;
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
    while (!quit) {
        pthread_mutex_lock(&mutex);
        client_info *pclient = dequeue();
        pthread_mutex_unlock(&mutex);
        if (pclient != NULL) {
            // thread can do work
            handle_connection(pclient);
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
        if ((rv = recv(p_client_socket->client_socket, &opcode, sizeof(uint8_t), 0)) == -1) 
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
            list_it(p_client_socket);
        }
        // Get Remote File
        else if (opcode == 0x04)
        {
            get_file(p_client_socket);
        }
        // Make Remote Directory
        else if (opcode == 0x05)
        {
            make_directory(p_client_socket);
        }
        // Put Remote File
        else if (opcode == 0x06)
        {
            put_file(p_client_socket);
        }
        
    }
    
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
    
    // create default admin user
    user *user1 = calloc(1, sizeof(*user1));

    memcpy(user1->uname, "admin", 5);
    memcpy(&user1->passwd, "password", 8);
    user1->perms = 3;

    // insert user into hashtable
    hash_table_insert(ht, user1);

    return ht;
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
    char full_path[MAX_NAME];
    memset(full_path, '\0', MAX_NAME);
    if (req.name_len == 0)
    {
        d = opendir(p_client_socket->dir);
        if (!d)
        {
            send(p_client_socket->client_socket, &fail, sizeof(uint8_t), 0);
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
            return -1;
        }
    }
    int counter = 0;
    memset(&resp, '\0', sizeof(resp));
    char content[content_len];

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
    char *filename = calloc(filename_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, filename, filename_len, 0)) == -1) 
    {
        free(filename);
        perror("recv");
        return -1;
    }

    char full_path[MAX_NAME];
    memset(full_path, '\0', MAX_NAME);
    memcpy(full_path, p_client_socket->dir, strlen(p_client_socket->dir));
    strncat(full_path, filename, strlen(filename));


    // open file, send 0 if it doesn't exist
    file_t *file_info = open_file(full_path, "r");
    if (!file_info) 
    {
        free(filename);
        server_response(p_client_socket, 0xff);
        return -1;
    }
    puts("file opened");

    // divide file size by packet size to get number of packets
    int packets = 0;
    fseek(file_info->fp, 0L, SEEK_END);
    uint32_t content_len = ftell(file_info->fp);
    rewind(file_info->fp);
    if (content_len > PACKET_SIZE) 
    {
        close_file(file_info);
        free(filename);
        server_response(p_client_socket, 0xff);
        return -1;
    } 


    
    // create buffer for contents of file to send
    char buff[content_len];

    rv = read_file(file_info, buff, content_len);
    if (!rv) 
    {
        close_file(file_info);
        free(filename);
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
    char *filename = calloc(filename_len + 1, 1);
    // receive the filename
    if ((rv = recv(p_client_socket->client_socket, filename, filename_len, 0)) == -1) 
    {
        free(filename);
        perror("recv");
        return -1;
    }

    char full_path[MAX_NAME];
    memset(full_path, '\0', MAX_NAME);
    memcpy(full_path, p_client_socket->dir, strlen(p_client_socket->dir));
    strncat(full_path, filename, strlen(filename));

    uint32_t content_len = ntohl(req.content_len);
    char content[content_len];
    
    // receive the file contents
    if ((rv = recv(p_client_socket->client_socket, &content, content_len, 0)) == -1) 
    {
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
        return -1;
    }

    // write contents to file and check for all bytes written
    int ret = write_file(file_info, content, content_len);
    if (ret != content_len)
    {
        close_file(file_info);
        server_response(p_client_socket, 0xff);
        free(filename);
        return -1;
    }
    
    server_response(p_client_socket, 1);
    puts("here");
    close_file(file_info);
    free(filename);

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
    char *dirname = calloc(dirname_len + 1, 1);

    if ((rv = recv(p_client_socket->client_socket, dirname, dirname_len, 0)) == -1) 
    {
        free(dirname);
        perror("recv");
        return -1;
    }

    char full_path[MAX_NAME];
    memset(full_path, '\0', MAX_NAME);
    memcpy(full_path, p_client_socket->dir, strlen(p_client_socket->dir));
    strncat(full_path, dirname, strlen(dirname));


    DIR *dir = opendir(full_path);
    if (dir)
    {
        free(dirname);
        closedir(dir);
        server_response(p_client_socket, 0x05);
    }

    if ((rv = mkdir(full_path, 0777)) == -1)
    {
        free(dirname);
        perror("mkdir");
        server_response(p_client_socket, 0xff);
    }

    server_response(p_client_socket, 0x01);
    free(dirname);
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
            server_response(p_client_socket, 0x01);
            free(username);
            free(password);
            free(user);
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
