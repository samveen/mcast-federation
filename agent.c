/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * agent.c
 *
 * Multicast agent core: Listens, receives, and sends: all in one.
 */

/* The usual suspects */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The usual suspects::network */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* We're timing sensitive */
#include <time.h>

/* We're threaded: Listen,multi-receive,send */
#include <pthread.h>
/* Because we're trying to make threads respond to socket events?? */
#include <sys/epoll.h>

/* Defines */
#define MSG_MAXLEN 255

/* As per https://tools.ietf.org/html/rfc5771#section-10, we can use anything in 239/8
 * so use 239.something
 */
#define MC_GROUP "239.0.0.1"
#define MC_PORT 6000


/* Create update message */
int build_update_message(char* message, size_t maxlen) {
    time_t t = time(0);
    return snprintf(message, maxlen, "time is %-24.24s", ctime(&t));
}

/* send thread */
void send_updates(struct sockaddr_in *addr) {
    int sock=0, cnt=0;
    char * message;
    message = (char*) malloc((size_t)MSG_MAXLEN);
    socklen_t addrlen = sizeof(*addr);
    addr->sin_addr.s_addr = inet_addr(MC_GROUP);
    while (1) {
        build_update_message(message, MSG_MAXLEN);
        printf("sending: %s\n", message);
        cnt = sendto(sock, message, sizeof(message), 0, (struct sockaddr *) addr, addrlen);
        if (cnt < 0) {
            perror("sendto");
            exit(1);
        }
        sleep(500);
    }
}

/* listen, receive */
/* TODO: Seperate into threads with evented I/O */
void listen_for_updates(struct sockaddr_in *addr) {
    int sock, cnt;
    socklen_t addrlen;
    struct ip_mreq mreq;
    char message[50];
    if (bind(sock, (struct sockaddr *) addr, sizeof(*addr)) < 0) {        
        perror("bind");
        exit(1);
    }    
    mreq.imr_multiaddr.s_addr = inet_addr(MC_GROUP);         
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt mreq");
        exit(1);
    }         
    while (1) {
        cnt = recvfrom(sock, message, sizeof(message), 0, (struct sockaddr *) addr, &addrlen);
        if (cnt < 0) {
            perror("recvfrom");
            exit(1);
        } else if (cnt == 0) {
            break;
        }
        printf("%s: message = \"%s\"\n", inet_ntoa(addr->sin_addr), message);
    }
}

/* TODO: Proper structure */
int main(int argc, char * argv[])
{
    struct sockaddr_in addr;
    int addrlen, sock, cnt;

    /* set up socket */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    memset((void *)&addr, 0x0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(MC_PORT);
    addrlen = sizeof(addr);

    /* Reuse */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt reuse");
        exit(1);
    }         
}
