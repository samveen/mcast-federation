/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 * Thanks: 
 *     http://stackoverflow.com/questions/2329571
 *
 * message_impl.c
 *
 * Implementations of functions to 
 * - Build messages to be published to the mcast group.
 * - Decode messages received from mcast peers.
 */

#include <curl/curl.h>

#include <inttypes.h>

#include "common.h"

#include "message.h"
#include "message_impl.h"

# include "curl_data.h"

rawdata_t* get_nginx_stats(void) {
    /* Message data */
    CURL *curl;
    CURLcode res;
    rawdata_t *data=NULL;

    curl = curl_easy_init();
    if(curl) {
        data=malloc(sizeof(rawdata_t));
        memset((char *)data, 0, sizeof(rawdata_t));

        curl_easy_setopt(curl, CURLOPT_URL, NGINX_STUBSTATS_URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, update_mycurl_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
        res = curl_easy_perform(curl);

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return (data);
}

int parse_rawdata_into_message(message_t* message,rawdata_t* data) {
    /* Example Data:
Active connections: 2 
server accepts handled requests
 617 617 60616 
Reading: 0 Writing: 1 Waiting: 1 
    */   
    char * iter=data->buffer;
    int64_t val; /* We shouldn't hit 18446744073709551615 cumulative requests anytime soon (2^64-1) */
    int count=0;
    for(int i=1; i<=7; ++i) {
        while (*iter && !(*iter>'0' && *iter <'9'))
            ++iter;
        count+=sscanf(iter,"%" SCNd64, &val);
        switch(i) {
            case 1:
                message->Connections=val;
                break;
            case 5:
                message->Reading=val;
                break;
            case 6:
                message->Writing=val;
                break;
            case 7:
                message->Waiting=val;
                break;
        }
    }
    return((count!=7)?count:0);
}


/* Compose the update message */
size_t build_message(message_t ** const message, const uint8_t msgtype) {

    rawdata_t *data=NULL;

    time_t ts = time(NULL);

    if (*message==NULL)
        *message = (message_t*) malloc(sizeof(message_t));

    /* Clean message */
    memset((void *)*message, 0, sizeof(message_t));

    /* Message properties */
    (*message)->mtype = msgtype;
    (*message)->mastery = mastery_rank;
    (*message)->mtimestamp = ts;

    switch (msgtype) {
        case MSG_NEWBIE:
        case MSG_UPDATE:
            data=get_nginx_stats();
            if(parse_rawdata_into_message((*message),data)!=0)
                break;
        case MSG_WR_ON:
        case MSG_WR_OFF:
            return (sizeof(message_t));
    }
    return 0;
}

int decode_message(char * buf, size_t size, message_t ** const message) {
    if (*message==NULL)
        *message = (message_t*) malloc(sizeof(message_t));
    if (size==sizeof(message_t)) {
        memcpy(*message, buf, size);
        return(0);
    } else 
        return (-1);
}

uint8_t get_message_type(const message_t * const msg) {
    return (msg->mtype);
}

uint32_t get_message_mastery(const message_t* const msg) {
    return (msg->mastery);
}

int64_t get_message_int64_value(const message_t* const msg) {
    return (msg->Connections);
}

void dump_message_to_stdout(const message_t * const message) {
    if (message!=NULL)
        printf("Connections: %" PRId64 ", Reading: %" PRId64 ", Writing: %" PRId64 ", Waiting: %" PRId64 "\n", message->Connections, message->Reading, message->Reading, message->Waiting);
    else
        printf("Message is null");
}

message_t * deep_copy(const message_t * const orig) {
    message_t * copy=(message_t *) malloc (sizeof(message_t));
    memcpy(copy,orig,sizeof(message_t));
    return(copy);
}

void deep_free(message_t * const junk) {
    return free(junk);
}
