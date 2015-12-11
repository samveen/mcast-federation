/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * listener.c
 *
 * Core epoll loop
 */
#include "common.h"

#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#include "message.h"

/* listen, receive */
void listener (void *arg)
{
    int sockfd, rv;

    sockfd = create_and_bind ();
    if (sockfd == -1)
        abort ();

    struct sockaddr peer;
    socklen_t len = sizeof(peer);
    int msgsize;
    char buf[BUFSIZE];
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    message_t *msg=NULL;

    /* The Blocking recvfrom loop */
    while (1) {
        msgsize = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&peer, (socklen_t *)&len);
        if (msgsize < 0) {
            perror("recvfrom");
            break;
        }
        rv = getnameinfo (&peer, len,
                          hbuf, sizeof hbuf,
                          sbuf, sizeof sbuf,
                          NI_NUMERICHOST | NI_NUMERICSERV);
        if (rv == 0) {
            printf("Message of size %d from (host=%s, port=%s): '%s'\n", msgsize, hbuf, sbuf, buf);
        }
        rv=decode_message(buf, msgsize, &msg);
        if (rv == 0) {
            printf("Message decoded successfully.\n");
            dump_message_to_stdout(msg);
        } else
            printf("Message decoding failure.\n");
    }

    close (sockfd);
}
