/**
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup xtimer
 * @{
 * @file
 * @brief xtimer posix wrapper
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <unistd.h>

#include "xtimer.h"

unsigned int sleep(unsigned int seconds)
{
    xtimer_usleep64(seconds*SEC_IN_USEC);
    return 0;
}

int usleep(useconds_t usec)
{
    xtimer_usleep64(usec);
    return 0;
}
