/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * curl_data.h
 *
 * Declaration of curl data specific functions.
 */
#ifndef _CURL_DATA_H
#define _CURL_DATA_H

/* Max message buffer len. Constant portion of stats is 90 chars
 * [root@spare01-a15 ~]# curl -s  http://nginx-status.localhost/nginx_status |sed 's/[0-9]//g' |wc -c
 * 90
 * The rest contains 4 currents stats and 3 cumulative numbers. Even if NginX gets 1 trillion
 * requests a day, we won't overflow the below limit for decades.
 * */
#define BUFFER_MAXLEN 8192

/* shared data def */
typedef struct {
    char *end;
    char buffer[BUFFER_MAXLEN];
} rawdata_t;

size_t update_mycurl_data(void *, size_t , size_t , rawdata_t *);

#endif
