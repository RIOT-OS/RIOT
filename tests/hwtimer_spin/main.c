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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "hwtimer.h"
#include "board.h"

int main(void)
{
    puts("This is just a functionality test for hwtimer_spin.");

    puts("\nYou should see the message \"success\" after a while if"
            " this test was successful.");
    puts("If you do not see that message, something went wrong.\n");

    for (unsigned long r = 1000; r > 0; r--) {
        for (unsigned long i = HWTIMER_SPIN_BARRIER; i > 0; i--) {
            hwtimer_wait(i);
        }
    }
    puts("success");
    return 0;
}
