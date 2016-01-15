/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * trigger_waiting_room.c
 *
 * Definition of functions to interact with waiting room.
 */
#include <curl/curl.h>
#include <string.h>

#include "curl_data.h"
#include "trigger_waiting_room.h"
#include "common.h"

int set_waiting_room_state(const char * command, const char * expected_result)
{
    int val=0;
    struct curl_slist *headers=NULL;

    CURL * handle = curl_easy_init(); 

    rawdata_t data;

    memset((char *)&data, 0, sizeof(rawdata_t));

    /* Content type */
    headers = curl_slist_append(headers, "Content-Type: text/plain;");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

    /* Postdata */
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, command);
 
    /* URL */
    curl_easy_setopt(handle, CURLOPT_URL, WAITING_ROOM_CONFIGURATION_URL);

    /* response storage */
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, update_mycurl_data);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);

    /* Do it */
    curl_easy_perform(handle);

    curl_slist_free_all(headers);

    curl_easy_cleanup(handle);

    val=strncmp(data.buffer,expected_result,strlen(expected_result));

    return (val);
}

int enable_waiting_room()
{
    return set_waiting_room_state("enable;", "Waiting room: Enabled");
}

int disable_waiting_room() 
{
    return set_waiting_room_state("disable;", "Waiting room: Disabled");
}
