/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 * @file    fd.c
 * @brief   Providing implementation for close for fds defined in fd.h.
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */
#include <errno.h>

#include "fd.h"
#include "unistd.h"

int usleep(useconds_t useconds)
{
    timex_t time = timex_set(0, useconds);
    timex_normalize(&time);
    vtimer_sleep(time);
    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    timex_t time = timex_set(seconds, 0);
    vtimer_sleep(time);
    return 0;
}

/**
 * @}
 */
