/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * create_and_bind.c
 *
 * socket creation code
 */
#include "common.h"

int create_and_bind () {
    struct addrinfo criteria;
    struct addrinfo *mcaddrset, *it;
    int rv, sock;

    memset (&criteria, 0, sizeof (struct addrinfo));
    criteria.ai_family = AF_UNSPEC;
    criteria.ai_socktype = SOCK_DGRAM;
    criteria.ai_protocol = IPPROTO_UDP;
    criteria.ai_flags |= AI_NUMERICHOST;

    rv = getaddrinfo (MC_GROUP, str(MC_PORT), &criteria, &mcaddrset);
    if (rv != 0) {
      fprintf (stderr, "getaddrinfo: %rv\n", gai_strerror (rv));
      return -1;
    }

    for (it = mcaddrset; it != NULL; it = it->ai_next) {
        sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (sock == -1)
            continue;

        rv = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
        if (rv < 0) {
            perror("setsockopt reuse");
            close (sock);
            continue;
        }

        rv = bind (sock, it->ai_addr, it->ai_addrlen);
        if (rv != 0) {
            perror("Bind");
            close (sock);
            continue;
        }

        if (it->ai_family == AF_INET6) {
            struct ipv6_mreq mreq;
            memcpy(&mreq.ipv6mr_multiaddr,
                   &((struct sockaddr_in6 *)it->ai_addr)->sin6_addr,
                   sizeof(struct in6_addr));
            mreq.ipv6mr_interface = 0;
            rv = setsockopt(sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq, sizeof(mreq));
            if (rv == 0)
                break;
            perror("setsockopt::ipv6");
        } else if (it->ai_family == AF_INET) {
            struct ip_mreq mreq;
            mreq.imr_multiaddr = ((struct sockaddr_in *) it->ai_addr)->sin_addr;
            mreq.imr_interface.s_addr = 0;
            rv = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
            if (rv == 0)
                break;
            perror("setsockopt::ipv4");
        } else {
            fprintf (stderr, "Unknown socket family \"%d\"\n",it->ai_family);
        }
        close (sock);
    }

    if (it == NULL) {
        fprintf (stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo (mcaddrset);

    return sock;
}
