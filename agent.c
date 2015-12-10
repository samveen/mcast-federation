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

/* Curl global init/cleanup */
#include <curl/curl.h>

#include "common.h"

pthread_t listener_thread, publisher_thread;

int main(int argc, char * argv[])
{
    /* We don't do parameters. Maybe that'll change in the future
     * FIXIT: We don't do any configurtion other than build time.
     */
    /* Global init */
    curl_global_init(CURL_GLOBAL_ALL);

    listener((void *) NULL);

    /* Global cleanup */
    curl_global_cleanup();
}
