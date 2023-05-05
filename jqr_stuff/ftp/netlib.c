/** @file netlib.c
 * 
 * @brief A network library for creating sockets, sending and receiving data, and closing sockets
 *
 * @par       
 * Stevne Silbert, stevensilbertjr@gmail.com
 */

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

#include "netlib.h"

#define BACKLOG 10

/*!
 * @brief Removes zombie processes.
 *
 * @param[in] s  Zombie process to be reaped.
 *
 * @return
 */
void
sigchld_handler(int s)
{
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


/*!
 * @brief Get IP address in IPv4 or IPv6 format.
 *
 * @param[in] sa  Sockaddr struct containing the address.
 *
 * @return IPv4 or IPv6 address.
 */
void *
get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


/*!
 * @brief Creates a socket for a client or server of TCP or UDP protocol.
 *
 * @param[in] type  Case-insensitive "TCP" or "UDP".
 * @param[in] port  Port to create socket.
 * @param[in] sockfd  Pointer to integer to store created socket file descriptor.
 * @param[in] node  IP address or Hostname for Client, NULL for server.
 * @param[in] ainfo  Struct containing info of server or remote client.
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
create_socket(char *type, const char *port, int *sockfd, char *node, struct addrinfo *ainfo)
{

    if (!type || !port) {
        fprintf(stderr, "NULL value not accepted for port or type");
        return -1;
    }

    if (!strcmp(type, "TCP") || !strcmp(type, "tcp")) {
        
        if (!node) {
            create_tcp_server(sockfd, port, ainfo);
        } else {
            create_tcp_client(sockfd, port, node, ainfo);
        }
    } else if (!strcmp(type, "UDP") || !strcmp(type, "udp")) {
        if (!node) {
            create_udp_server(sockfd, port, ainfo);
        } else {
            create_udp_client(sockfd, port, node, ainfo);
        }

    } else {
        fprintf(stderr, "Invalid type\n");
    }

    return 0;
}


/*!
 * @brief Creates a UDP server socket.
 *
 * @param[in] sockfd  Pointer to integer to store created socket file descriptor.
 * @param[in] port  Port to create socket.
 * @param[in] ainfo  Struct storing server info
 * 
 * @return Status code, 0 for success and negative for error code.
 */
int
create_udp_server(int *sockfd, const char *port, struct addrinfo *ainfo)
{
    struct addrinfo hints, *res, *p;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    for (p = res; p != NULL; p = p->ai_next) {
        if ((*sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("client:socket");
            continue;
        }
        if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(*sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind socket\n");
        return -1;
    }

    memcpy(ainfo, p, sizeof(*p));
    freeaddrinfo(res);

    return 0;
}


/*!
 * @brief Creates a UDP client socket.
 *
 * @param[in] sockfd  Pointer to integer to store created socket file descriptor.
 * @param[in] port  Port to create socket.
 * @param[in] node  Server address or hostname
 * @param[in] ainfo  Struct storing server info
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
create_udp_client(int *sockfd, const char *port, char *node, struct addrinfo *ainfo)
{
    struct addrinfo hints, *res, *p;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(node, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    for (p = res; p != NULL; p = p->ai_next) {
        if ((*sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("client:socket");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to create socket\n");
        return -1;
    }
    memcpy(ainfo, p, sizeof(*p));
    freeaddrinfo(res);

    return 0;
}


/*!
 * @brief Creates a TCP server socket.
 *
 * @param[in] sockfd  Pointer to integer to store created socket file descriptor.
 * @param[in] port  Port to create socket.
 * @param[in] ainfo  Struct storing server info
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
create_tcp_server(int *sockfd, const char *port, struct addrinfo *ainfo)
{
    struct addrinfo hints, *res, *p;
    int rv;
    int yes=1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    for (p = res; p != NULL; p = p->ai_next) {
        if ((*sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("server:socket");
            continue;
        }
        if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
        }
        if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(*sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "failed to bind socket\n");
        return -1;
    }
    memcpy(ainfo, p, sizeof(*p));
    freeaddrinfo(res);

    struct sigaction sa;

    if (listen(*sockfd, BACKLOG) == -1) {
        perror("listen");
        return -1;
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return -1;
    }

    printf("Server: waiting for connections...\n");


    return 0;
}


/*!
 * @brief Creates a TCP client socket.
 *
 * @param[in] sockfd  Pointer to integer to store created socket file descriptor.
 * @param[in] port  Port to create socket.
 * @param[in] node  Server address or hostname
 * @param[in] ainfo  Struct storing server info
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
create_tcp_client(int *sockfd, const char *port, char *node, struct addrinfo *ainfo)
{
    struct addrinfo hints, *res, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(node, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    for (p = res; p != NULL; p = p->ai_next) {
        if ((*sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("server:socket");
            continue;
        }
        if (connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(*sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return -1;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);

    memcpy(ainfo, p, sizeof(*p));
    freeaddrinfo(res);

    return 0;

}


/*!
 * @brief Sents data via TCP or UDP.
 *
 * @param[in] data  Data to be sent
 * @param[in] size  Size of data being sent
 * @param[in] socket  File descriptor of connection
 * @param[in] ainfo  Struct storing destination information 
 *
 * @return The number of bytes sent.
 */
int
send_stuff(char *data, size_t size, int socket, struct addrinfo *ainfo)
{
    int numbytes;
    struct sockaddr addr;
    socklen_t len = sizeof(addr);
    
    if (!ainfo) {
        if (send(socket, data, size, 0) == -1) {
            fprintf(stderr, "Send Error\n");
            return -1;
        }
    } else if (ainfo->ai_socktype == SOCK_DGRAM) {
        getpeername(socket, &addr, &len);
        addr.sa_family = AF_INET;
        if ((numbytes = sendto(socket, data, strlen(data), 0, ainfo->ai_addr, ainfo->ai_addrlen)) == -1) {
            fprintf(stderr, "Sendto error\n");
            return -1;
        }
    } else {
        fprintf(stderr, "Invalid type\n");
        return -1;
    }
    return numbytes;
}


/*!
 * @brief Receives data via TCP or UDP.
 *
 * @param[in] buffer  Buffer to store received data.
 * @param[in] socket  File descriptor of connection.
 * @param[in] ainfo  Struct storing client or server info.
 *
 * @return The number of bytes received.
 */
int
receive_stuff(void *buffer, int socket, struct addrinfo *ainfo)
{
    int numbytes;
    char s[INET6_ADDRSTRLEN];

    if (!ainfo) {
        if ((numbytes = recv(socket, buffer, strlen(buffer)-1, 0)) == -1) {
            fprintf(stderr, "recv Error\n");
            return -1;
        }
    } else if (ainfo->ai_socktype == SOCK_DGRAM) {
        socklen_t addr_len;
        struct sockaddr_storage their_addr;
        if ((numbytes = recvfrom(socket, buffer, strlen(buffer)-1, 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            fprintf(stderr, "recvfrom error\n");
            return -1;
        }
        printf("listener: got packet from %s\n", inet_ntop(their_addr.ss_family, 
        get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s)));    
        
    } else {
        fprintf(stderr, "Invalid type\n");
        return -1;
    }
    return numbytes;
}


/*!
 * @brief Closes a socket with file descriptor.
 *
 * @param[in] socket  File descriptor for socket to be closed.
 *
 * @return Status code, 0 for success and negative for error code.
 */
int
close_socket(int socket)
{
    if (!socket) {
        fprintf(stderr, "Invalid Socket File Descriptor\n");
        return -1;
    }
    close(socket);
    return 0;
}