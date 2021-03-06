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


typedef struct peerinfo_s peerinfo_t;

typedef struct hashtable_s hashtable_t;

typedef struct hashtable_iterator_s hashtable_iterator_t;

struct hashtable_iterator_s {
    hashtable_t * hashtable;
    size_t current_bin;
    peerinfo_t * current_entry;
    in_addr_t (*next)(hashtable_iterator_t *it);
};

hashtable_t *ht_create(size_t);
void ht_set(hashtable_t *, in_addr_t, const message_t *const);
const message_t * ht_get(hashtable_t *, in_addr_t);
void ht_remove(hashtable_t *, in_addr_t);

hashtable_iterator_t * get_iterator(hashtable_t*);
#endif
