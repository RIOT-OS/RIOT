/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup posix
 *
 * @{
 * @file
 * @brief  posix sleep/usleep implementation wrapped around @ref ztimer
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <unistd.h>

#include "kernel_defines.h"
#include "timex.h"
#include "ztimer.h"

unsigned int sleep(unsigned int seconds)
{
    ztimer_sleep(ZTIMER_MSEC, seconds * MS_PER_SEC);
    return 0;
}

int usleep(useconds_t usec)
{
    ztimer_sleep(ZTIMER_USEC, usec);
    return 0;
}
