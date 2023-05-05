
#ifndef NETLIB_H
#define NETLIB_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

// helper function for child processes
void sigchld_handler(int );
// helper function to return either ipv4 or ipv6 address
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *);
int create_socket(const char *, const char *, int *, char *, struct addrinfo *, int);
int create_udp_server(int *sockfd, const char *port, struct addrinfo *ainfo);
int create_udp_client(int *sockfd, const char *port, char *node, struct addrinfo *ainfo);
int create_tcp_server(int *sockfd, const char *port, struct addrinfo *ainfo, int time);
int create_tcp_client(int *sockfd, const char *port, char *node, struct addrinfo *ainfo);
int send_stuff(char *, size_t, int, struct addrinfo *ainfo);
int receive_stuff(void *, int, struct addrinfo *ainfo);
int close_socket(int);

#endif