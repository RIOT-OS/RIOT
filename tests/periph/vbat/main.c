/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for backup battery monitoring
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "board.h"
#include "ztimer.h"
#include "periph/vbat.h"

int main(void)
{
    puts("\nRIOT backup battery monitoring test\n");
    puts("This test will sample the backup battery once a second\n\n");

    int32_t bat_mv;
    while (1) {
        if ((bat_mv = vbat_sample_mv()) < 0) {
            return 1;
        }
        printf("VBAT: %"PRIi32"[mV]\n", bat_mv);
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }
    return 0;
}
