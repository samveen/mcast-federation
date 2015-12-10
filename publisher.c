/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * publish_update.c
 *
 * Publisher core: gets updates, and pushed them out to the mcast group.
 */
#include <arpa/inet.h>

#include "common.h"

#include "message.h"
#include "message_impl.h"

/* send thread */
void send_updates(struct sockaddr_in *addr) {
    int sock=0, cnt=0;
    message_t *msg;
    msg = (message_t*) malloc(sizeof(message_t));
    socklen_t addrlen = sizeof(*addr);
    addr->sin_addr.s_addr = inet_addr(MC_GROUP);
    while (1) {
        build_message(msg, MSG_UPDATE);
        cnt = sendto(sock, msg, sizeof(message_t), 0, (struct sockaddr *) addr, addrlen);
        if (cnt < 0) {
            perror("sendto");
            exit(1);
        }
        sleep(500);
    }
}

