/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sixlowdriver.h"

#ifdef BORDER_TESTING
#include "testing.h"
#endif

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s r_addr if_name tty_dev\n", argv[0]);
        return -1;
    }

    char addr[IPV6_ADDR_LEN];
    sprintf(addr, "abcd::1034:00FF:FE00:%s/64", argv[1]);
    char if_name[IF_NAME_LEN];
    strncpy(if_name, argv[2], IF_NAME_LEN);
    char tty_dev[DEV_LEN];
    strncpy(tty_dev, argv[3], DEV_LEN);

    if (border_initialize(if_name, addr, tty_dev) == 0) {
#ifdef BORDER_TESTING
        char ping_addr[IPV6_ADDR_LEN];
        float interval;

        if (argc < 9) {
            fprintf(stderr, "Usage: %s r_addr if_name tty_dev ping_id result_dir skeleton_file ping_count interval\n", argv[0]);
            return -1;
        }

        sscanf(argv[8], "%f", &interval);
        sprintf(ping_addr, "abcd::%s/64", argv[4]);

        start_test(ping_addr, argv[5], argv[6], atoi(argv[7]), interval);
#else

        while (1);

#endif
    }

    return 0;
}
