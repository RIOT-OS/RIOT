/*
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
    puts("This is just a functionality test for hwtimer_spin.");

    for (unsigned long r = 10000; r > 0; r--) {
        for (unsigned long i = 6; i > 0; i--) {
            hwtimer_wait(i);
        }
    }
    puts("success");
    return 0;
}
