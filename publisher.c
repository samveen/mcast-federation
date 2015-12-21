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

/* send thread */
void publisher(void *arg) {
    int sock=0, cnt=0;
    size_t msgsize=0;
    message_t *msg=NULL;

    if ((sock=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
        perror("socket");
        return;
    }
   
    struct sockaddr_in addr;

    /* set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(MC_GROUP);
    addr.sin_port=htons(MC_PORT);

    while (1) {
        msgsize = build_message(&msg, MSG_UPDATE);
        cnt = sendto(sock,msg,msgsize,0,(struct sockaddr *) &addr,sizeof(addr));
        if (cnt < 0) {
            perror("sendto");
            exit(1);
        }
        printf("message of %zd bytes published\n",msgsize);
        sleep(10);
    }
}

