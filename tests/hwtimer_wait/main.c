/*
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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
 * @brief Hwtimer test application
 *
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "board_uart0.h"
#include "posix_io.h"
#include "hwtimer.h"
#include "thread.h"

int main(void)
{
    puts("This is a regression test for a race condition in hwtimer_wait.");
    puts("When the race condition is hit, the timer will wait for a very very long time...");

    for (unsigned long r = 10000; r > 0; r--) {
        for (unsigned long i = 256; i; i = i >> 1) {
            hwtimer_wait(i);
        }
    }
    puts("success");
    return 0;
}
