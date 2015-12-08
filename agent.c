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

#include "common.h"

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

/* TODO: Proper structure */
int main(int argc, char * argv[])
{
    listener((void *) NULL);
}
