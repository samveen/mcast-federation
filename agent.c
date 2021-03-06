/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * agent.c
 *
 * Multicast agent core: Listens, receives, and sends: all in one.
 */

/* The usual suspects */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* The usual suspects::network */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* We're timing sensitive */
#include <time.h>

/* No threading. Lets do supervisor, listener, publisher processes */
// #include <pthread.h>
/* Because we're trying to make threads respond to socket events?? */
#include <sys/epoll.h>

/* Curl global init/cleanup */
#include <curl/curl.h>

#include "common.h"
#include "options.h"


uint8_t whoami;

/* PIDs for signalling  */
pid_t supervisor; /* set once in parent via getpid instead of getppid in every child*/
pid_t pidl=0, pidp=0; /* for supervisor to signale children */

/* signaling variable: Set by signal handlers */
uint8_t state;

/* NO BACKGROUNDING, OR CLOSING OF STREAMS */

void sighandler(int signum, siginfo_t *info, void *data)
{
    int status;
    union sigval sig_value;
    sig_value.sival_int=0;

    switch(whoami) {
        case MONITOR:
            write(STDERR_FILENO, "Signal received :", sizeof("Signal received :"));

            switch(signum) {
                case SIGCHLD:
                    write(STDERR_FILENO, "SIGCHLD.\n", sizeof("SIGCHLD.\n"));
                    if (info->si_pid==pidl) {
                        waitpid(pidl,&status,WNOHANG);
                        pidl=0;
                    } else if (info->si_pid==pidp) {
                        waitpid(pidp,&status,WNOHANG);
                        pidp = 0;
                    }
                    break;
                case SIGHUP:
                    write(STDERR_FILENO, "SIGHUP.\n", sizeof("SIGHUP.\n"));
                    break;
                case SIGUSR1:
                    break;
                case SIGUSR2:
                    kill(pidp,SIGUSR2);
                    break;
                default:
                    switch(signum-SIGRTMIN) {
                        case 5:
                            sigqueue(pidp,SIGRTMIN+5,sig_value);
                            break;
                        case 6:
                            sigqueue(pidp,SIGRTMIN+6,sig_value);
                            break;
                        default:
                            write(STDERR_FILENO, "Unhandled signal.\n", sizeof("Unhandled signal.\n"));
                    }
            }
            break;

        case LISTENER:
            switch(signum) {
                case SIGUSR1:
                state=STATE_NORMAL;  // FIXME
                break;
            }
            break;

        case PUBLISHER:
            switch(signum) {
                case SIGUSR1:
                    state=STATE_KILLME;
                    break;
                case SIGUSR2:
                    break;
                default:
                    switch(signum-SIGRTMIN) {
                        case 5:
                            state=STATE_ENABLEWR;
                            break;
                        case 6:
                            state=STATE_DISABLEWR;
                            break;
                    }
            }
            break;
    }
}

void init_signals(void)
{
    int signals[] = { SIGHUP, SIGCHLD, SIGUSR1, SIGUSR2, SIGRTMIN+5, SIGRTMIN+6,0 };
    state=1;

    struct sigaction sa;

    memset(&sa,0,sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);

	sa.sa_flags = SA_SIGINFO|SA_RESTART;
    sa.sa_sigaction = sighandler;

    for (int * i=signals ; (*i)!=0 ; ++i ) {
        if (sigaction((*i), &sa, NULL) == -1) {
            fprintf(stderr, "sigaction failed on signal (%d)", (*i));
            return;
        }
    }
}

int main(int argc, char * argv[])
{
    /* We don't do parameters. Maybe that'll change in the future
     * FIXIT: We don't do any configurtion other than build time.
     */
    supervisor=getpid();
    whoami=MONITOR;

    init_signals();

    // Parse parameters
    parse_options(argc, argv);

    prctl(PR_SET_NAME, "agent:Monitor",0,0,0);
    memcpy(argv[0], "agent:Monitor",sizeof("agent:Monitor"));
    while(state) {
        if(pidl == 0) { // We dont have a listener pid or it's dead
            if ((pidl=fork())==0) {
                whoami=LISTENER;
                state=STATE_NEWBIE;
                prctl(PR_SET_NAME, "agent:Listener",0,0,0);
                memcpy(argv[0], "agent:Listener",sizeof("agent:Listener"));
                // Receiver
                listener((void *) NULL);
                printf("Listener Exiting\n");
                return(0);
            }

        }

        if(pidp == 0) { // We dont have a publisher pid or it's dead.
            if ((pidp=fork())==0) {
                // Publisher
                whoami=PUBLISHER;
                state=STATE_NORMAL;
                prctl(PR_SET_NAME, "agent:Publisher",0,0,0);
                memcpy(argv[0], "agent:Publisher",sizeof("agent:Publisher"));
                /* Global init */
                curl_global_init(CURL_GLOBAL_ALL); 

                publisher((void *) NULL);

                /* Global cleanup */
                curl_global_cleanup();
                printf("Publisher Exiting\n");
                return(0);
            }
        }
        pause();
    }
}
