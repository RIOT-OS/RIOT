/*
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
    printf("when the race condition is hit, hwtimer will wait a very very long time...\n");

    while (1) {
        for (unsigned long i = 256; i; i = i >> 1) {
            printf("wait %lu\n", i);
            hwtimer_wait(i);
        }
    }
}
