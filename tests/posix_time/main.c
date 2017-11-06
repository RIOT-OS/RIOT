/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Posix sleep test application
 *
 * @author Christian Mehlis <mehlis@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <unistd.h>

int main(void)
{
    puts("usleep 1 x 1000*1000");
    for (int i = 0; i < 10; i++) {
        useconds_t us = i*1000u*1000u;
        printf("calling usleep(%u)\n", (unsigned int) us);
        usleep(us);
        puts("wake up");
    }

    puts("sleep 1");
    for (int i = 0; i < 10; i++) {
        unsigned int s = i;
        printf("calling sleep(%u)\n", s);
        sleep(s);
        puts("wake up");
    }

    puts("done");
    return 0;
}
