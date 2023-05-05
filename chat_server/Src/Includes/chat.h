#ifndef CHAT_H
#define CHAT_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <pthread.h>

#include "netlib.h"
#include "file_lib.h"
#include "pool_queue.h"
#include "my_ht.h"

#define MAX_NAME 255

extern pthread_mutex_t mutex;

typedef struct user_request user_request;
typedef struct lobby_request lobby_request;
typedef struct chat_request chat_request;
typedef struct serv_response serv_response;
typedef struct delete_f delete_f;
typedef struct ls_request ls_request;
typedef struct ls_response ls_response;
typedef struct get_request get_request;
typedef struct get_response get_response;
typedef struct put_request put_request;
typedef struct mkdir_request mkdir_request;

typedef struct {
    struct hashtable *ht;
    struct lobbies *lobs;
    int client_socket;
    uint32_t session_id;
    uint8_t perms;
    char dir[MAX_NAME];
    char uname[MAX_NAME];
} __attribute__((__packed__)) client_info;

typedef struct {
    char passwd[MAX_NAME];
    char uname[MAX_NAME];
    uint8_t perms;
} __attribute__((__packed__)) user;

struct lobbies
{
    struct lobby *head;
    int size;
};

struct lobby
{
    struct lobby *next;
    struct client_list *clients;
    char *lobby_name;
    int name_len;
    int lobby_num;
    int lobby_size;
};

struct client_list
{
    struct client *head;
};

struct client
{
    char *name;
    int fd;
    int name_len;
    uint8_t lobby_num;
    struct client *next;
};


struct lobbies *create_lobbies(void);
int lobby_operation(client_info *p_client_socket);
int create_lobby(client_info *p_client_socket, lobby_request *req);
void list_lobbies(client_info *p_client_socket, lobby_request *req);
void enter_lobby(client_info *p_client_socket, lobby_request *req);
void destroy_lobby(client_info *p_client_socket, lobby_request *req);
void destroy_lobbies(struct lobbies *l);

void send_all(client_info *p_client_socket, chat_request *req);
void send_direct(client_info *p_client_socket, chat_request *req);
int chat_operation(client_info *p_client_socket);
void exit_room(client_info *p_client_socket, chat_request *req);
void list_members(client_info *p_client_socket, chat_request *req);

void *handle_connection(client_info *p_client_socket);
void *thread_function(void *arg);
int parse_request(char *buff, user *user);
hash_table *get_creds(void);
void
write_creds(hash_table *ht);

int list_it(client_info *p_client_socket);
int get_file(client_info *);
int put_file(client_info *p_client_socket);
int delete_remote(client_info *p_client_socket);
int make_directory(client_info *p_client_socket);

int user_operation(client_info *p_client_socket);
int user_login(client_info *p_client_socket, user_request *req);
int create_user(client_info *p_client_socket, user_request *req, const int permission);
int delete_user(client_info *p_client_socket, user_request *req);


void server_response(client_info *p_client_socket, uint8_t code);





#endif