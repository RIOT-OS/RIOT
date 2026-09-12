/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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
