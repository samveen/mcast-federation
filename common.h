/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * common.h
 *
 * Core shared definitions 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXEVENTS 64

/* As per https://tools.ietf.org/html/rfc5771#section-10, we can use anything in 239/8
 * so use 239.something
 */
#define MC_GROUP "224.0.0.1"
#define MC_PORT 6000

/* As per IPv4 protocol definition, max UDP packet size is as follows.
 */
#define BUFSIZE 65507

/* Peer information Hash size */
#define PEER_HASH_SIZE 128

/* Waiting room limits */
#define WAITING_ROOM_DISABLE_THRESHOLD 5
#define WAITING_ROOM_ENABLE_THRESHOLD 10

/* Waiting room state */
#define WAITING_ROOM_DISABLED 0
#define WAITING_ROOM_ENABLED  1

#define WAITING_ROOM_CONFIGURATION_URL "http://nginx-status.localhost/waiting_room_config"

/* Waiting room state changing commands */
#define WAITING_ROOM_DISABLE_COMMAND "curl --data-ascii 'disable;' -H 'Content-type: text/plain;'  http://nginx-status.localhost/waiting_room_config"
#define WAITING_ROOM_ENABLE_COMMAND  "curl --data-ascii 'enable;'  -H 'Content-type: text/plain;'  http://nginx-status.localhost/waiting_room_config"


/* Convenient conversion of constants to strings from
 * https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
 */ 
#define str(s) xstr(s)
#define xstr(s) #s

int create_and_bind (void);
void listener (void *);

void publisher(void *);
