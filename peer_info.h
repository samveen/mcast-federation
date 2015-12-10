/*
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * peer_info.h
 *
 * Data structures to host peer info
 */
#ifndef __PEER_INFO_H
#define __PEER_INFO_H 1

#include <netinet/in.h>
#include "message.h"

struct peerinfo_s {
	message_t *value;
	struct peerinfo_s *next;
        
	in_addr_t key; // IPv4 Forever 
};

typedef struct peerinfo_s peerinfo_t;
#endif
