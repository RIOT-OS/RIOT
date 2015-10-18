/*
 * Copyright (C) 2015 Mohmmad Ayman <Mohmmad.Salah94@eng-st.cu.edu.eg>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       Implementation of getopt lib.
 * @author      Mohmmad Ayman <Mohmmad.Salah94@eng-st.cu.edu.eg>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

/**
 * @getopt libirary light implementaion
 */
typedef struct {
    /* Global argument pointer. */
    char *optarg;
    /* Global argv index. */
    int optind;
} opt_r;

int getopt(int argc, char *argv[], char *optstring, opt_r *r)
{
    r->optarg = NULL;
    r->optind = 0;
    char *scanptr = NULL;
    char *place = NULL;
    char c;
        if(!scanptr || *scanptr =='\0')
        {
            scanptr = argv[r->optind]+1;
            r->optind++;
        }
        if (r->optind == 0 || argv[r->optind][0] != '-' || !argv[r->optind][1])
        {
            return -1;
        }
        if (argv[r->optind][1] == '-' && !argv[r->optind][2])
        {
            r->optind++;
            return -1;
        }
    c = *scanptr++;
    place = strchr(optstring, c);

        if (!place || c == ':') {
            fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
            return '?';
        }

    place++;
        if (*place == ':') {
            if (*scanptr != '\0') {
                r->optarg = scanptr;
                scanptr = NULL;
            } else if( r->optind < argc ) {
                r->optarg = argv[r->optind];
                r->optind++;
            } else {
                fprintf(stderr, "%s: option requires argument -%c\n", argv[0], c);
                return ':';
            }
        }
    return c;
}
