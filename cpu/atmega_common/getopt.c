/* This file is subject to the terms and conditions of the GNU Lesser
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


char *optarg;
int optind = 0;
char *scanptr = NULL;
int getopt(int argc, char *argv[], char *optstring)
{
char *place;
char c;
optarg = NULL;
        if(!scanptr || *scanptr =='\0')
        {
            scanptr = argv[optind]+1;
            optind++;   
        }
        if (optind == 0 || argv[optind][0] != '-' || !argv[optind][1])
        {
            return -1;
        }
        if (argv[optind][1] == '-' && !argv[optind][2])
        {
            optind++;
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
            optarg = scanptr;
            scanptr = NULL;
        } else if( optind < argc ) {
            optarg = argv[optind];
            optind++;
        } else {
            fprintf(stderr, "%s: option requires argument -%c\n", argv[0], c);
            return ':';
        }
    }
    return c;
}