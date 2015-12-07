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

struct peerinfo {
    int id;
    char data[32];
};

typedef struct peerinfo peerinfo_t;
#endif
