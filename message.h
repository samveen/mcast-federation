/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * message.h
 *
 * Declaration of non-implementation specific messages. The actual
 * implementations should go into message_impl.h
 */
#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <stdint.h>
#include <time.h>

#define MSG_NEWBIE 1u
#define MSG_UPDATE 2u

/* Forward declaration */
struct message;

typedef struct message message_t;

int build_message( message_t * const, const uint8_t);

/* Actual implementation in message_impl.h:
 *
 * struct message {
 *     uint8_t mtype;
 *     uint8_t padding[3]; // Future resistant (unfortunately not future proof)
 * 
 *     time_t  mtimestamp;
 * 
 *     int64_t message_specific_data[NEEDED_LEN]
 * };
*/
#endif
