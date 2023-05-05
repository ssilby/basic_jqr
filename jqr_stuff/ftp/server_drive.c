/**
* FTP server code
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>

#include "netlib.h"
#include "file_lib.h"
#include "pool_queue.h"
#include "my_ht.h"
#include "server.h"

#define TABLE_SIZE 20
#define MAXDATASIZE 1000
#define SERVERPORT "4444"
#define THREAD_POOL_SIZE 20
#define UNUSED(x) (void)(x)

pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

volatile sig_atomic_t quit = 0;

void 
handler(int signum, siginfo_t * info, void *ptr)
{
    UNUSED(signum);
    UNUSED(info);
    UNUSED(ptr);
    quit = 1;
}

int 
main(int argc, char *argv[])
{

    struct sigaction sa;
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction SIGINT");
    }

    // create threads to hande future connections
    for (int i = 1; i < THREAD_POOL_SIZE; i++) 
    {
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }

    hash_table *ht = get_creds();

    if (!ht) 
    {
        printf("Hash Table creation error\n");
    }

    user *user1 = (user *) hash_table_lookup(ht, "phteven");
    user *user2 = (user *) hash_table_lookup(ht, "admin");

    if (!user1) 
    {
        printf("silby did a bad :(\n");
    }

    printf("user1: uname: %s passwd: %s perms: %u\n", user1->uname, user1->passwd, user1->perms);
    printf("user2: uname: %s passwd: %s perms: %u\n", user2->uname, user2->passwd, user2->perms);


    struct addrinfo *ainfo = malloc(sizeof(*ainfo));

    if (!ainfo) 
    {
        fprintf(stderr, "Ran out of memory\n");
    }

    int client_socket;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];


    int sockfd, numbytes;
    char buf[MAXDATASIZE];

    if (argc < 2 || argc > 3) 
    {
        fprintf(stderr, "please provide type\n");
        return -1;
    }
    
    
    
    
    int rv = 0;

    if (argc == 2) 
    {
        rv = create_socket(argv[1], SERVERPORT, &sockfd, NULL, ainfo);
    } 
    else if (argc == 3) 
    {
        rv = create_socket(argv[1], SERVERPORT, &sockfd, argv[2], ainfo);
    }

    if (rv != 0) 
    {
        fprintf(stderr, "error creating socket\n");
        free(ainfo);
        return -1;
    }
    
    if (ainfo->ai_socktype == SOCK_STREAM)
    {
        while(!quit) 
        {
            client_socket = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
            if (client_socket == -1) 
            {
                perror("accept");
                break;
            }
            // inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
            // printf("server: got connection from %s\n", s);
            client_info *client = malloc(sizeof(*client));
            if (!client) 
            {
                fprintf(stderr, "ran out of memory");
                continue;
            } 
            client->ht = malloc(sizeof(ht));
            client->client_socket = client_socket;
            client->ht = ht;
            
            pthread_mutex_lock(&mutex);
            enqueue(client);
            pthread_mutex_unlock(&mutex);
        }
    } else 
    {
        fprintf(stderr, "Invalid type\n");
        free(ainfo);
        return -1;
    }
    
    for (int i = 0; i < THREAD_POOL_SIZE; i++) 
    {
        pthread_join(thread_pool[i], NULL);
    }    
    free(ainfo);
    close(sockfd);
    close(client_socket);

    

    return 0;
}