/* 
 * Copyright 2015, Samveen
 * See LICENSE file for terms
 *
 * options.c
 *
 * Command line option handling code
 */

#include <getopt.h>
#include <stdlib.h>
#include <inttypes.h>

#include "common.h"

static struct option opts[]= {
             { .name="disable_threshold", .has_arg=1, .flag=NULL, .val='d'},
             { .name="enable_threshold",  .has_arg=1, .flag=NULL, .val='e'},
             { .name="mastery",           .has_arg=1, .flag=NULL, .val='m'},
             { .name="help",              .has_arg=0, .flag=NULL, .val='h'},
             { .name=0,                   .has_arg=0, .flag=NULL, .val=0  }
};

void usage(const char* name)
{
    fprintf(stderr,"Usage: %s {-d|--disable_threshold} DTH {-e|--enable_threshold} ETH {-m|--msatery} MAR | {-h|-?|--help}\n\
            -d      Disable threshold (average value per node)\n\
            -e      Enable threshold (average value per node)\n\
            -m      Mastery rank (Current highest is master)\n\
            -h      Help (This message)\n",
            name);
}

int parse_options(int argc, char* argv[])
{
    int c, done=0x00;
    int index = 0;

    while (done!=0x07 && optind < argc) {
        c = getopt_long(argc, argv, "d:e:m:h?", opts, &index);
        if (c == -1)
            break;
        switch (c) {
            case 'd':
                done|=0x01;
                wr_disable_threshold=strtoull(optarg, NULL, 10);
                printf("Disable Threshold %" PRId64 "\n", wr_enable_threshold);
                break;

            case 'e':
                done|=0x02;
                wr_enable_threshold=strtoull(optarg, NULL, 10);
                printf("Enable Threshold %" PRId64 "\n", wr_enable_threshold);
                break;

            case 'm':
                done|=0x04;
                mastery_rank=atoi(optarg);
                printf("Mastery %d\n",mastery_rank);
                break;

            case '?':
            case 'h':
                usage(argv[0]);
                exit(0);

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf(". Ignoring.\n");
    }

    if (done!=0x07) {
        printf("All options not received.\n");
        exit(-1);
    }
    return (0);
}
