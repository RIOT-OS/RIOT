/*
 * Copyright (C) 2014-17 Freie Universität Berlin
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
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

/* needed for posix usleep */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

#include <stdio.h>
#include <unistd.h>

int main(void)
{
    puts("5 x usleep(i++ * 500000)");
    for (unsigned i = 0; i < 5; i++) {
        useconds_t us = i * 500000u;
        usleep(us);
        puts("wake up");
    }

    puts("5 x sleep(i++)");
    for (unsigned i = 0; i < 5; i++) {
        sleep(i);
        puts("wake up");
    }

    puts("DONE");
    return 0;
}
