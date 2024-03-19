/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       Test the SAUL interface of devices by periodically reading from
 *              them
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#include <stdio.h>

#include "flash_utils.h"
#include "phydat.h"
#include "saul_reg.h"
#include "xtimer.h"

/**
 * @brief   Read th sensors every second
 */
#define INTERVAL            (1LU * US_PER_SEC)

int main(void)
{
    phydat_t res;
    xtimer_ticks32_t last_wakeup = xtimer_now();

    puts("SAUL test application");

    while (1) {
        saul_reg_t *dev = saul_reg;

        if (dev == NULL) {
            puts("No SAUL devices present");
            return 1;
        }

        while (dev) {
            int dim = saul_reg_read(dev, &res);
            printf("\nDev: %s\tType: %" PRIsflash "\n", dev->name,
                   saul_class_to_str(dev->driver->type));
            phydat_dump(&res, dim);
            dev = dev->next;
        }
        puts("\n##########################");

        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
    }

    return 0;
}
