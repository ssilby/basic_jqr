#include <stdio.h>

#include "netlib.h"

#define SERVERPORT "4444"
#define MAXBUFLEN 100


int main(int argc, char *argv[])
{

    char hostname[20];
    gethostname(hostname, 20);
    printf("server hostname: %s\n", hostname);


    struct addrinfo *ainfo = malloc(sizeof(*ainfo));
    if (!ainfo) {
        fprintf(stderr, "Ran out of memory\n");
    }

    int new_fd;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];


    if (argc < 2 || argc > 3) {
        fprintf(stderr, "please provide type\n");
        return -1;
    }
    
    int sockfd; 
    char data[100] = "Echo server speaking, how may I help you?";
    int rv = 0;

    if (argc == 2) {
        rv = create_socket(argv[1], SERVERPORT, &sockfd, NULL, ainfo);
    } else if (argc == 3) {
        rv = create_socket(argv[1], SERVERPORT, &sockfd, argv[2], ainfo);
    }

    if (!rv) {
        printf("socket #: %d\n", sockfd);
    } else {
        fprintf(stderr, "error creating socket\n");
        free(ainfo);
        return -1;
    }

    char rbuff[100];

    if (ainfo->ai_socktype == SOCK_STREAM) {
        while(1) { // main accept() loop
            sin_size = sizeof their_addr;
            new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
            if (new_fd == -1) {
                free(ainfo);
                perror("accept");
                continue;
            }
            
            int optval, optlen, s2;
            getsockopt(s2, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen);
            printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

            inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
            printf("server: got connection from %s\n", s);

            /*if (!fork()) { // this is the child process
                close(sockfd); // child doesn't need the listener
                if (send_stuff(data, strlen(data), new_fd, ainfo) == -1) {
                    free(ainfo);
                    perror("send");
                }
                close(new_fd);
                free(ainfo);
                exit(0);
            }
            */
            if (send_stuff(data, strlen(data), new_fd, ainfo) == -1) {
                    free(ainfo);
                    perror("send");
                }
            if (receive_stuff(rbuff, new_fd, ainfo) == -1) {
                    free(ainfo);
                    perror("send");
                }
            if (!strcmp("quit", rbuff)) {
                puts("quitting");
                free(ainfo);
                close(new_fd);
                exit(1);
            }
            if (send_stuff(rbuff, strlen(rbuff), new_fd, ainfo) == -1) {
                free(ainfo);
                perror("send");
            }
        }
        close(new_fd);
    } else if (ainfo->ai_socktype == SOCK_DGRAM) {
        char buf[MAXBUFLEN];
        int numbytes;
        printf("listener: waiting to recvfrom...\n");
        if ((numbytes =receive_stuff(buf, sockfd, ainfo)) == -1) {
                free(ainfo);
                perror("recv");
                exit(1);
        }
        printf("listener: packet is %d bytes long\n", numbytes);
        buf[numbytes] = '\0';
        printf("listener: packet contains \"%s\"\n", buf);
        close(sockfd);
    } else {
        fprintf(stderr, "Invalid type\n");
        free(ainfo);
        return -1;
    }
    
    free(ainfo);
    return 0;
}

// socket()
// send()
// recv()
// sendto()
// recvfrom()
// bind()
// connect()
// accept()
// getsockopt()
// setsockopt()
// getaddrinfo()
// gethostname()
// struct sockaddr
// struct sockaddr_in
// struct sockaddr_un