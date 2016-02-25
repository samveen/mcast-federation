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

#include <inttypes.h>

#include "message.h"
#include "peer_info.h"
#include "trigger_waiting_room.h"

/* Determine who's master (Thank you, Warframe) */
/* The live agent with the largest mastery rank is the master. must mark itself as such */
uint32_t mastery_rank;

/* Cluster master */
uint32_t cluster_master;

/* Waiting room limits */
uint64_t wr_disable_threshold;
uint64_t wr_enable_threshold;

/* Timeout for data expiration */
time_t expiration_timeout=30;

/* listen, receive */
int listener (void *arg)
{
    int sockfd, rv;
    uint32_t mr;

    sockfd = create_and_bind ();
    if (sockfd == -1)
        return(-1);

    int waiting_room_status=WAITING_ROOM_DISABLED;

    struct sockaddr peer;
    socklen_t len = sizeof(peer);
    int msgsize;
    char buf[BUFSIZE];
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    message_t *msg=NULL;
    hashtable_t * messages = ht_create(PEER_HASH_SIZE);
    struct in_addr key;
    uint64_t total, count;

    /* The Blocking recvfrom loop */
    do {
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
            printf("Message of size %d from (host=%s, port=%s)\n", msgsize, hbuf, sbuf);
            inet_pton(AF_INET, hbuf, (void *)&key);
        }
        rv=decode_message(buf, msgsize, &msg);
        if (rv == 0) {
            printf("Message decoded successfully: Message ");
            //dump_message_to_stdout(msg);
            switch(get_message_type(msg)) {
                case MSG_NEWBIE: // Same as update,(with bonus content)
                    printf("NEWBIE/");
                    kill(supervisor, SIGUSR2);
                case MSG_UPDATE:
                    printf("UPDATE\n");
                    ht_set(messages,key.s_addr,msg);
                    break;
                case MSG_WR_ON:
                    printf("WR_ON: Enabling waiting room\n");
                    if (waiting_room_status!=WAITING_ROOM_ENABLED) {
                        waiting_room_status=WAITING_ROOM_ENABLED;
                        enable_waiting_room();
                    }
                    break;
                case MSG_WR_OFF:
                    printf("WR_OFF: Disabling waiting room\n");
                    if(waiting_room_status!=WAITING_ROOM_DISABLED) {
                        waiting_room_status=WAITING_ROOM_DISABLED;
                        disable_waiting_room();
                    }
                    break;
            }
        } else
            printf("Message decoding failure.\n");

        /* We got a message (valid/invalid, no matter) */
        hashtable_iterator_t * it=get_iterator(messages);
        time_t cts = time(NULL);
        total=0;
        count=0;
        cluster_master=0;
        while((key.s_addr=it->next(it))!=0) {
            const message_t* m=ht_get(messages, key.s_addr);
            if(cts-get_message_timestamp(m) > expiration_timeout) { /* invalidation check */
                ht_remove(messages, key.s_addr);
                inet_ntop(AF_INET, (void *)&key, hbuf, NI_MAXHOST-1);
                printf("Invalidating data for host %s\n", hbuf);
            } else {
                mr=get_message_mastery(m);
                if(mr>cluster_master) /* determing master */
                    cluster_master=mr;
                total+=get_message_int64_value(m);
                ++count;
            }
        }

        printf("Master('%u') and  I('%u').\n",cluster_master,mastery_rank);

        if(mastery_rank==cluster_master) {
            printf("Cluster Average: %" PRId64 "\n", (total/count));
            if(waiting_room_status==WAITING_ROOM_DISABLED && (total/count)>=wr_enable_threshold) {
                printf("Send Enable message\n");
                kill(supervisor, SIGRTMIN+5);
            } else if (waiting_room_status==WAITING_ROOM_ENABLED && (total/count)<=wr_disable_threshold) {
                printf("Send Disable message\n");
                kill(supervisor, SIGRTMIN+6);
            }
        }
    } while (1);

    close (sockfd);
    return (0);
}
