/**
* FTP server code
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "netlib.h"
#include "file_lib.h"
#include "pool_queue.h"
#include "my_ht.h"
#include "server.h"



#define TABLE_SIZE 20
#define MAXDATASIZE 1000
#define SERVERPORT "4444"
#define THREAD_POOL_SIZE 20
#define PACKET_SIZE 1024

// Struct to store username, password and user permissions (1 for admin and 2 for regular user)

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


void *
handle_connection(client_info *p_client_socket)
{
    user *creds = calloc(1, sizeof(*creds));
    while (!quit) 
    {
        if (p_client_socket->client_socket == -1) 
        {
            close(p_client_socket->client_socket);
            return NULL;
        }

        int rv;
        int pass_failure = 3;
        int user_failure = 4;
        if ((rv = recv(p_client_socket->client_socket, creds, sizeof(*creds), 0)) == -1) 
        {
            perror("recv");
            return NULL;
        }
        printf("uname: %s\npass: %s\nperms: %u\n", creds->uname, creds->passwd, creds->perms);
        
        user *new_user = (user *) hash_table_lookup(p_client_socket->ht, creds->uname); 
        // if not in table send welcome message again
        if (!new_user) 
        {
            puts("user not found");
            send(p_client_socket->client_socket, &user_failure, sizeof(pass_failure), 0);
            continue;
        }
        if (!strncmp(creds->passwd, new_user->passwd, 256)) 
        {
            printf("%s login success\n", creds->uname);
            send(p_client_socket->client_socket, &new_user->perms, sizeof(new_user->perms), 0);
            break;
        } 
        else 
        {
            puts("wrong password");
            send(p_client_socket->client_socket, &pass_failure, sizeof(pass_failure), 0);
            continue;
        }
        
    }
    while(!quit) 
    {
        if (p_client_socket->client_socket == -1) 
        {
            close(p_client_socket->client_socket);
            return NULL;
        }

        int rv;
        packet_h header;
        memset(&header, '\0', sizeof(header));
        if ((rv = recv(p_client_socket->client_socket, &header, sizeof(header), 0)) == -1) 
        {
            perror("recv");
            return NULL;
        } 
        else if (rv == 0) 
        {
            printf("%s closed connection\n", creds->uname);
            return NULL;
        }


        int opt = header.code;
        switch (opt) 
        {
            // GET
            case 1:
                get_file(p_client_socket, &header);
                break;
            // PUT
            case 2:
                break;
            // DIR
            case 3:
                // function to send directory filenames to client
                list_it(p_client_socket);
                break;
            // ADD
            case 4:
                break;
            // DEL
            case 5:
                break;
            // QUIT
            case 6:
                break;
        }
    }
}


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
                return NULL;
            }
            rv = read_file(accounts, entry, sizeof(*entry));
            // break when struct is null
            if (rv == 0) 
            {
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
        user1->perms = 1;
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


int 
list_it(client_info *p_client_socket)
{
    int count = 0;
    packet_h header;
    memset(&header, '\0', sizeof(header));
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) 
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type != DT_DIR) 
            {
                count++;
            }
        }
        // send number of files in directory
        send(p_client_socket->client_socket, &count, sizeof(count), 0);
        // receive acknowledgement and check number of files
        recv(p_client_socket->client_socket, &header, sizeof(header), 0);
        if (header.code != 7) 
        {
            printf("error");
            return -1;
        }
        if (count != header.pay_len) 
        {
            printf("error");
            return -1;
        }
        // open directory to get list of filenames
        closedir(d);
        // open present directory
        d = opendir(".");
        // ignore directories
        while ((dir = readdir(d)) != NULL) 
        {
            if (dir->d_type != DT_DIR) 
            {
                // get length of file dict entry and put in string buffer
                int dir_len = snprintf(NULL, 0, "%s", dir->d_name);
                char *buffer = calloc(dir_len + 1, 1);
                snprintf(buffer, dir_len + 1, "%s", dir->d_name);
                int paylen = strlen(buffer);
                // send length of filename
                send(p_client_socket->client_socket, &paylen, sizeof(paylen), 0);
                // send filename
                send(p_client_socket->client_socket, buffer, strlen(buffer), 0);
                free(buffer);
            }
        }

        
        closedir(d);
    }
    return 0;
}

void 
get_file(client_info *p_client_socket, packet_h *header)
{
    int rv;
    packet_h fail_header = {0, 0};
    char *buffer = calloc(header->pay_len, 1);
    if (!buffer) 
    {
        fprintf(stderr, "not enough memory");
        return;
    }
    if ((rv = recv(p_client_socket->client_socket, buffer, header->pay_len, 0)) == -1) 
    {
        perror("recv");
        return;
    }

    //DEBUG
    printf("filename: %s\n", buffer);

    // open file, send 0 if it doesn't exist
    file_t *file_info = open_file(buffer, "r");
    if (!file_info) 
    {
        send(p_client_socket->client_socket, &fail_header, sizeof(fail_header), 0);
        return;
    }
    puts("file opened");
    // divide file size by packet size to get number of packets
    int packets = 0;
    struct stat *attribs = get_attrib(file_info);
    if (attribs->st_size < PACKET_SIZE) 
    {
        packets = 1;
    } 
    else 
    {
        packets = attribs->st_size / PACKET_SIZE + (attribs->st_size % PACKET_SIZE != 0);
    }

    packet_h send_header = {packets, PACKET_SIZE};
    // send header with number of packets and size
    send(p_client_socket->client_socket, &send_header, sizeof(send_header), 0);
    
    // receive ack of number of packets
    int ack_code = 0;
    if ((rv = recv(p_client_socket->client_socket, &ack_code, sizeof(ack_code), 0)) == -1) 
    {
        perror("recv");
        return;
    }
    // if packets don't match send fail
    if (ack_code != packets) 
    {
        send(p_client_socket->client_socket, &fail_header, sizeof(fail_header), 0);
        return;
    }
    // read file to packet size buffers and send. Continue after receiving ACK
    char *buff[PACKET_SIZE];
    packet_h ack_header = {0, 0};
    for (int i = 0; i < packets; ++i) 
    {  
        memset(&ack_header, '\0', sizeof(ack_header));
        memset(buff, '\0', PACKET_SIZE);
        rv = read_file(file_info, buff, PACKET_SIZE);
        if (!rv) 
        {
            send(p_client_socket->client_socket, &fail_header, sizeof(fail_header), 0);
            return;
        }
        send(p_client_socket->client_socket, buff, rv, 0);
        recv(p_client_socket->client_socket, &ack_header, sizeof(ack_header), 0);
    }
    close_file(file_info);
    return;
}