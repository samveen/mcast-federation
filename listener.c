/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * listener.c
 *
 * Core epoll loop
 */
#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

static int make_socket_non_blocking (int fd) {
    int flags, rv;

    flags = fcntl (fd, F_GETFL, 0);
    if (flags == -1) {
        perror ("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    rv = fcntl (fd, F_SETFL, flags);
    if (rv == -1) {
        perror ("fcntl");
        return -1;
    }

    return 0;
}

/* listen, receive */
/* TODO: Seperate into threads with evented I/O */
void listener (void *arg)
{
    int sockfd, rv;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;

    sockfd = create_and_bind ();
    if (sockfd == -1)
        abort ();

    rv = make_socket_non_blocking (sockfd);
    if (rv == -1)
        abort ();

    efd = epoll_create1 (0);
    if (efd == -1) {
        perror ("epoll_create");
        abort ();
    }

    event.data.fd = sockfd;
    event.events = EPOLLIN | EPOLLET;
    rv = epoll_ctl (efd, EPOLL_CTL_ADD, sockfd, &event);
    if (rv == -1) {
        perror ("epoll_ctl");
        abort ();
    }

    /* Buffer where events are returned */
    events = calloc (MAXEVENTS, sizeof event);

    /* The event loop */
    while (1) {
        int n, i;

        n = epoll_wait (efd, events, MAXEVENTS, -1);
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                /* An error has occured on this fd, or the socket is not
                   ready for reading (why were we notified then?) */
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
            } else if (sockfd == events[i].data.fd) {
                /* We have a notification on the listening socket, which
                   means one or more incoming connections. */
                while (1) {
                    struct sockaddr peer;
                    socklen_t len = sizeof(peer);
                    int msgsize;
                    char buf[BUFSIZE];
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    msgsize = recvfrom(sockfd, buf, BUFSIZE, 0, (struct sockaddr *)&peer, (socklen_t *)&len);
                    if (msgsize < 0) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            break;
                        } else {
                            perror("recvfrom");
                            break;
                        }
                    }
                    buf[msgsize]='\0';
                    rv = getnameinfo (&peer, len,
                            hbuf, sizeof hbuf,
                            sbuf, sizeof sbuf,
                            NI_NUMERICHOST | NI_NUMERICSERV);
                    if (rv == 0) {
                        printf("Message of size %d from (host=%s, port=%s): '%s'\n", msgsize, hbuf, sbuf, buf);
                    }
                }
                continue;
            } else {
                /* We have data on the fd waiting to be read. Read and
                   display it. We must read whatever data is available
                   completely, as we are running in edge-triggered mode
                   and won't get a notification again for the same
                   data. */
                int done = 0;

                while (1) {
                    ssize_t count;
                    char buf[512];

                    count = read (events[i].data.fd, buf, sizeof buf);
                    if (count == -1) {
                        /* If errno == EAGAIN, that means we have read all
                           data. So go back to the main loop. */
                        if (errno != EAGAIN) {
                            perror ("read");
                            done = 1;
                        }
                        break;
                    } else if (count == 0) {
                        /* End of file. The remote has closed the
                           connection. */
                        done = 1;
                        break;
                    }

                    /* Write the buffer to standard output */
                    rv = write (1, buf, count);
                    if (rv == -1) {
                        perror ("write");
                        abort ();
                    }
                }

                if (done) {
                    printf ("Closed connection on descriptor %d\n",
                            events[i].data.fd);

                    /* Closing the descriptor will make epoll remove it
                       from the set of descriptors which are monitored. */
                    close (events[i].data.fd);
                }
            }
        }
    }

    free (events);
    close (sockfd);
}
