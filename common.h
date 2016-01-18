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
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#define MONITOR 1u
#define LISTENER 2u
#define PUBLISHER 4u


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
extern uint64_t wr_disable_threshold;
extern uint64_t wr_enable_threshold;
#define WAITING_ROOM_DISABLE_THRESHOLD 5
#define WAITING_ROOM_ENABLE_THRESHOLD 10

/* Waiting room state */
#define WAITING_ROOM_DISABLED 0
#define WAITING_ROOM_ENABLED  1

#define WAITING_ROOM_CONFIGURATION_URL "http://nginx-status.localhost/waiting_room_config"

/* Waiting room state changing commands */
//#define WAITING_ROOM_DISABLE_COMMAND "curl --data-ascii 'disable;' -H 'Content-type: text/plain;'  http://nginx-status.localhost/waiting_room_config"
//#define WAITING_ROOM_ENABLE_COMMAND  "curl --data-ascii 'enable;'  -H 'Content-type: text/plain;'  http://nginx-status.localhost/waiting_room_config"

/* Process state */
#define STATE_QUIT   0u
#define STATE_NEWBIE 1u
#define STATE_NORMAL 2u
#define STATE_KILLME 4u
#define STATE_ENABLEWR 8u
#define STATE_DISABLEWR 16u
#define STATE_MASTER 128u

extern uint8_t state;


/* Convenient conversion of constants to strings from
 * https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
 */ 
#define str(s) xstr(s)
#define xstr(s) #s

/* Who's your Daddy */
extern pid_t supervisor;

/* Cluster state */
extern uint32_t greatest_cluster_size;
extern uint32_t current_cluster_size;

 /* Determine who's master */
extern uint32_t mastery_rank; // my rank
extern uint32_t cluster_master;

int create_and_bind (void);
int listener (void *);

int publisher(void *);
