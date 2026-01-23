/*
 * SPDX-FileCopyrightText: 2014-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
