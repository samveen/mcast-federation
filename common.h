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
#include <netdb.h>

#define MAXEVENTS 64

/* As per https://tools.ietf.org/html/rfc5771#section-10, we can use anything in 239/8
 * so use 239.something
 */
#define MC_GROUP "239.0.0.1"
#define MC_PORT 6000

/* As per IPv4 protocol definition, max UDP packet size is as follows.
 */
#define BUFSIZE 65507

/* Max mesg len: IS this useful?? */
#define MSG_MAXLEN 255

/* Convenient conversion of constants to strings from
 * https://gcc.gnu.org/onlinedocs/cpp/Stringification.html
 */ 
#define str(s) xstr(s)
#define xstr(s) #s
