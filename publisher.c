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

/* */
int newbie=1;
unsigned int a_little_bit=10;

/* send thread */
int publisher(void *arg) {
    int sock=0, cnt=0;
    size_t msgsize=0;
    message_t *msg=NULL;

    if ((sock=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        perror("socket");
        return(-1);
    }
   
    struct sockaddr_in addr;

    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(MC_GROUP);
    addr.sin_port=htons(MC_PORT);

    /* First: newbie message */
    msgsize = build_message(&msg, MSG_NEWBIE);
    cnt = sendto(sock,msg,msgsize,0,(struct sockaddr *) &addr,sizeof(addr));
    if (cnt < 0) {
        perror("sendto");
        return(-2);
    }
    printf("message of %zd bytes published\n",msgsize);
    sleep(a_little_bit);

    /* Updates */
    do {
        switch (state) {
            case STATE_ENABLEWR:
                msgsize = build_message(&msg, MSG_WR_ON);
                state=STATE_NORMAL;
                break;
            case STATE_DISABLEWR:
                msgsize = build_message(&msg, MSG_WR_OFF);
                state=STATE_NORMAL;
                break;
            case STATE_NORMAL:
                msgsize = build_message(&msg, MSG_UPDATE);
        }
        cnt = sendto(sock,msg,msgsize,0,(struct sockaddr *) &addr,sizeof(addr));
        if (cnt < 0) {
            perror("sendto");
            return(-2);
        }
        printf("message of %zd bytes published\n",msgsize);
        sleep(a_little_bit);
    } while (state);
    return(0);
}

