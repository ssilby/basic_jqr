#ifndef SERVER_H
#define SERVER_H

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

#define MAX_NAME 20

extern pthread_mutex_t mutex;


typedef struct {
    struct hashtable *ht;
    int client_socket;
} client_info;

typedef struct {
    char passwd[64];
    char uname[MAX_NAME];
    uint8_t perms;
} user;

typedef struct {
    int code;
    int pay_len;
} packet_h;



void *handle_connection(client_info *p_client_socket);
void *thread_function(void *arg);
int parse_request(char *buff, user *user);
hash_table *get_creds(void);
int list_it(client_info *);
void get_file(client_info *, packet_h *);


#endif