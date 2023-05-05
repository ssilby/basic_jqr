/** @file server.c
 * 
 * @brief a driver to create the chat server. Spins off a client to a new thread
 * every time a new one connects.
 * @par       
 * Steven Silbert, stevensilbertjr@gmail.com
 * 
 * Server Requirements




Written in C


Uses a thread pool to handle data sent to the server


3-10 workers, created dynamically or at run time


Workers can be designated specific tasks or can handle all tasks equally


Requires a client to log in or create an account when connecting


User accounts require username and password


Usernames and passwords are stored and validated during login


The server must have multiple lobbies for users to join, this can be a set amount of lobbies or you can allow users to create lobbies


Must transmit messages and files between clients appropriately


Verification should be implemented as seen fit to ensure files and messages are received properly


Must make good use of data structures


Logs all events in the server (new account, connections, disconnects, messages, file transfers, private messages, etc.)
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "../Includes/netlib.h"
#include "../Includes/file_lib.h"
#include "../Includes/pool_queue.h"
#include "../Includes/chat.h"
#include "../Includes/my_ht.h"

#define TABLE_SIZE 20
#define MAXDATASIZE 1000
#define SERVERPORT "4444"
#define THREAD_POOL_SIZE 10
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

    // catch interrupt signal to close threads
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction SIGINT");
    }

    // create threads to hande future connections
    for (int i = 0; i < THREAD_POOL_SIZE; i++) 
    {
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }

    // create hashtable
    hash_table *ht = get_creds();

    if (!ht) 
    {
        printf("Hash Table creation error\n");
    }

    // check that default user is created and inserted into table
    user *user1 = (user *) hash_table_lookup(ht, "admin");

    if (!user1) 
    {
        printf("silby did a bad :(\n");
    }

    printf("user1: uname: %s passwd: %s perms: %u\n", user1->uname, user1->passwd, user1->perms);

    int opt; 
    int time_flag = 0; 
    int port_flag = 0; 
    int dir_flag = 0; 
    long int timeout;
    char *port;
    char *directory;
    char *ptr;
    while ((opt = getopt(argc, argv, ":p:d:t:")) != -1)
    {
        switch(opt)
        {
            case 'p':
                port_flag = 1;
                port = strndup(optarg, strlen(optarg));
                if (!port)
                {
                    fprintf(stderr, "not enough memory");
                    return -1;
                }
                break;
            case 'd':
                dir_flag = 1;
                directory = strndup(optarg, strlen(optarg));
                if (!directory)
                {
                    fprintf(stderr, "not enough memory");
                    return -1;
                }
                break;
            case 't':
                time_flag = 1;
                timeout = strtol(optarg, &ptr, 10);
                break;
            case '?':
                fprintf(stderr, "invalid option: -%c\n", optopt);
                return -1; 
        }
    }

    if (port_flag == 0)
    {
        port = strndup(SERVERPORT, strlen(SERVERPORT));
    }

    if (time_flag == 0)
    {
        timeout = 600;
    }
    if (dir_flag == 0)
    {
        fprintf(stderr, "absolute path to ftp server directory required\n");
        return -1;
    }

    printf("Port: %s\n", port);
    printf("Directory: %s\n", directory);
    printf("Timeout: %ld\n", timeout);

    char last = directory[strlen(directory) - 1];

    if (last != '/')
    {
        directory = realloc(directory, strlen(directory) + 1);
        strncat(directory, "/", 2);
    }
    

    struct addrinfo *ainfo = malloc(sizeof(*ainfo));

    if (!ainfo) 
    {
        fprintf(stderr, "Ran out of memory\n");
    }

    int client_socket;
    struct sockaddr_storage their_addr = {'\0'};
    socklen_t sin_size = {'\0'};

    int sockfd;
    
    // create socket
    int rv = 0;
    int time = (int) timeout;
    

    rv = create_socket("tcp", port, &sockfd, NULL, ainfo, time);

    if (rv != 0) 
    {
        fprintf(stderr, "error creating socket\n");
        free(ainfo);
        return -1;
    }
    
    uint32_t session_id = 1;

    struct lobbies *Lobbies = create_lobbies();
    if (!Lobbies)
    {
        fprintf(stderr, "ran out of memory");
        return -1;
    }

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
        client_info *client = calloc(sizeof(*client), 1);
        if (!client) 
        {
            fprintf(stderr, "ran out of memory");
            continue;
        }

        

        client->lobs = Lobbies;
        client->client_socket = client_socket;
        client->ht = ht;
        client->session_id = session_id;
        client->perms = 0;
        memset(client->uname, '\0', sizeof(client->uname));
        
        memcpy(&client->dir, directory, strlen(directory));
        pthread_mutex_lock(&mutex);
        enqueue(client);
        pthread_mutex_unlock(&mutex);

        session_id++;
    }


    for (int i = 0; i < THREAD_POOL_SIZE; i++) 
    {
        pthread_join(thread_pool[i], NULL);
    }    

    write_creds(ht);

    free(ainfo);
    close(sockfd);
    close(client_socket);
    destroy_ht(ht);
    destroy_lobbies(Lobbies);
    free(Lobbies);
    free(directory);
    free(port);
    return 0;
}