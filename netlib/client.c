#include <stdio.h>

#include "netlib.h"

#define MAXDATASIZE 100
#define SERVERPORT "4444"

int main(int argc, char *argv[])
{
    struct addrinfo *ainfo = malloc(sizeof(*ainfo));
    if (!ainfo) {
        fprintf(stderr, "Ran out of memory\n");
    }

    int sockfd, numbytes;
    char buf[MAXDATASIZE];

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "please provide type\n");
        return -1;
    }
    
    char data[100] = "Hello, world!";
    int rv = 0;

    if (argc == 2) {
        rv = create_socket(argv[1], SERVERPORT, &sockfd, NULL, ainfo);
    } else if (argc == 3) {
        rv = create_socket(argv[1], SERVERPORT, &sockfd, argv[2], ainfo);
    }
    printf("socket %d\n", rv);
    if (!rv) {
        printf("socket #: %d\n", sockfd);
    } else {
        fprintf(stderr, "error creating socket\n");
        free(ainfo);
        return -1;
    }

    if (ainfo->ai_socktype == SOCK_STREAM){
        if ((numbytes = receive_stuff(buf, sockfd, ainfo)) == -1) {
            perror("recv");
            free(ainfo);
            exit(1);
        }
        printf("client: received '%s'\n", buf);
    } else if (ainfo->ai_socktype == SOCK_DGRAM) {
        char message[MAXDATASIZE] = "You successfully received a datagram message!";
        if ((numbytes = send_stuff(message, strlen(message), sockfd, ainfo)) == -1) {
            perror("talker: sendto");
            free(ainfo);
            exit(1);
        }
        printf("numbytes = %d\n", numbytes);
        printf("%s: sent\n", message);
        close(sockfd);
    } else {
        fprintf(stderr, "Invalid type\n");
        free(ainfo);
        return -1;
    }
    

    
    free(ainfo);
    close(sockfd);

    

return 0;
}