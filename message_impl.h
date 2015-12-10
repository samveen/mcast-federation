/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * message_impl.h
 *
 * Implementation specific portions of messages
 */
#ifndef __MESSAGE_IMPL_H
#define __MESSAGE_IMPL_H

#include "message.h"

/* This particular message implementation is used to share NginX stub_stats
 * data in the federation.The stub stats URL is usually configured as a vhost
 * on the loopback interface. The data returned by the URL
 */
#define NGINX_STUBSTATS_URL "http://nginx-status.localhost/nginx_status"
/* Max stats message len. Constant portion of stats is 90 chars
 * [root@spare01-a15 ~]# curl -s  http://nginx-status.localhost/nginx_status |sed 's/[0-9]//g' |wc -c
 * 90
 * The rest contains 4 currents stats and 3 cumulative numbers. Even if NginX gets 1 trillion
 * requests a day, we won't overflow the below limit for decades.
 * */
#define STATS_MAXLEN 8192

struct message {
    uint8_t mtype;
    uint8_t padding[7]; /* Future resistant (unfortunately not future proof)*/

    time_t  mtimestamp;

    int64_t Connections;
    int64_t Reading;
    int64_t Writing;
    int64_t Waiting;
};

#endif
